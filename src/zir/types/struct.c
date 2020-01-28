#include <fllib.h>
#include <stdlib.h>
#include "struct.h"

static void member_free(void *ptr)
{
    if (!ptr)
        return;

    struct ZirStructTypeMember *member = (struct ZirStructTypeMember*) ptr;
    
    if (member->name)
        fl_cstring_free(member->name);

    fl_free(member);
}

struct ZirStructType* zir_type_struct_new(char *name)
{
    struct ZirStructType *type = fl_malloc(sizeof(struct ZirStructType));
    type->base.typekind = ZIR_TYPE_STRUCT;
    type->name = name != NULL ? fl_cstring_dup(name) : NULL; // If NULL is an anonymous struct
    type->members = fl_list_new_args((struct FlListArgs) {
        .value_cleaner = member_free
    });

    return type;
}

void zir_type_struct_add_member(struct ZirStructType *struct_type, const char *name, struct ZirType *member_type)
{
    struct ZirStructTypeMember *member = fl_malloc(sizeof(struct ZirStructTypeMember));

    member->name = fl_cstring_dup(name);
    member->type = member_type;

    fl_list_append(struct_type->members, member);
}

struct ZirStructTypeMember* zir_type_struct_get_member(struct ZirStructType *struct_type, const char *name)
{
    struct FlListNode *tmp = fl_list_head(struct_type->members);
    while (tmp != NULL)
    {
        struct ZirStructTypeMember *member = (struct ZirStructTypeMember*) tmp->value;
        
        if (flm_cstring_equals(name, member->name))
            return member;

        tmp = tmp->next;
    }

    return NULL;
}

unsigned long zir_type_struct_hash(struct ZirStructType *type)
{
    static const char *format = "[struct][n:%s]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, type->name);

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

/*
 * Function: zir_type_to_string
 *  This function has the added complexity of taking into account if 
 *  the type is an array and its size, therefore we need to use a heap
 *  allocated string, but we benefit from the <type_string_mapping_pool> variable
 *  to reuse strings.
 */
char* zir_type_struct_to_string(struct ZirStructType *type)
{
    if (type == NULL)
        return NULL;

    unsigned long type_hash = zir_type_struct_hash(type);

    if (type->base.to_string.value == NULL)
    {
        // First call, initialize the value
        type->base.to_string.value = fl_cstring_new(0);
    }
    else if (type_hash == type->base.to_string.version)
    {
        // If the type information didn't change, just return the 
        // current value
        return type->base.to_string.value;
    }

    char *string_value = NULL;
    
    if (type->name != NULL)
    {
        string_value = fl_cstring_dup(type->name);
    }
    else
    {
        string_value = fl_cstring_dup("{ ");

        struct FlListNode *tmp = fl_list_head(type->members);
        while (tmp != NULL)
        {
            struct ZirStructTypeMember *member = (struct ZirStructTypeMember*) tmp->value;
            fl_cstring_vappend(&string_value, "%s: %s", member->name, member->type ? zir_type_to_string(member->type) : "<unknown>");

            tmp = tmp->next;

            if (tmp)
                fl_cstring_append(&string_value, ", ");
        }

        fl_cstring_append(&string_value, " }");
    }
    
    // Update the string representation
    type->base.to_string.version = type_hash;
    type->base.to_string.value = fl_cstring_replace_realloc(type->base.to_string.value, type->base.to_string.value, string_value);

    fl_cstring_free(string_value);

    return type->base.to_string.value;
}

bool zir_type_struct_structurally_equals(struct ZirStructType *type_a, struct ZirStructType *type_b)
{
    if (fl_list_length(type_a->members) != fl_list_length(type_b->members))
        return false;

    struct FlListNode *a_node = fl_list_head(type_a->members);    

    while (a_node)
    {
        struct ZirStructTypeMember *a_member = (struct ZirStructTypeMember*) a_node->value;

        struct FlListNode *b_node = fl_list_head(type_b->members);
        bool a_exists_in_b = false;
        while (b_node)
        {
            struct ZirStructTypeMember *b_member = (struct ZirStructTypeMember*) b_node->value;

            if (flm_cstring_equals(a_member->name, b_member->name))
            {
                bool equals = false;
                if (a_member->type->typekind != ZIR_TYPE_STRUCT || b_member->type->typekind != ZIR_TYPE_STRUCT)
                    equals = zir_type_equals(a_member->type, b_member->type);
                else
                    equals = zir_type_struct_structurally_equals((struct ZirStructType*) a_member->type, (struct ZirStructType*) b_member->type);
                
                if (equals)
                {
                    a_exists_in_b = true;
                    break;
                }
            }

            b_node = b_node->next;
        }

        if (!a_exists_in_b)
            return false;

        a_node = a_node->next;
    }

    return true;
}

bool zir_type_struct_equals(struct ZirStructType *type_a, struct ZirType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZirType*) type_a == type_b;

    if (type_b->typekind != ZIR_TYPE_STRUCT)
        return false;

    struct ZirStructType *type_b_struct = (struct ZirStructType*) type_b;
    
    if (type_a->name != NULL && type_b_struct->name != NULL)
        return flm_cstring_equals(type_a->name, type_b_struct->name);
    else if (type_a->name != type_b_struct->name) // If one of them has a name, they are not equals
        return false;

    // Structural equality
    return zir_type_struct_structurally_equals(type_a, type_b_struct);
}

bool zir_type_struct_is_assignable_from(struct ZirStructType *target_type, struct ZirType *from_type)
{
    if (!target_type || !from_type)
        return false;

    if (from_type->typekind != ZIR_TYPE_STRUCT)
        return false;

    struct ZirStructType *struct_from_type = (struct ZirStructType*) from_type;

    if (target_type->name != NULL && struct_from_type->name != NULL)
        return flm_cstring_equals(target_type->name, struct_from_type->name);

    // Structural equality (we can safely convert between struturally equals objects)
    return zir_type_struct_structurally_equals(target_type, struct_from_type);
}

bool zir_type_struct_is_castable_to(struct ZirStructType *struct_type, struct ZirType *target_type)
{
    if (struct_type == NULL || target_type == NULL)
        return false;

    if (target_type->typekind != ZIR_TYPE_STRUCT)
        return false;

    // If we can assign the struct to a target type instance, we can cast to it too
    return zir_type_is_assignable_from(target_type, (struct ZirType*) struct_type);
}

size_t zir_type_struct_size(struct ZirStructType *type)
{
    if (!type)
        return 0;
    
    // FIXME: Add the size of the members
    return 0;
}

void zir_type_struct_free(struct ZirStructType *type)
{
    if (!type)
        return;

    if (type->base.to_string.value != NULL)
        fl_cstring_free(type->base.to_string.value);

    if (type->name)
        fl_cstring_free(type->name);

    if (type->members)
        fl_list_free(type->members);

    fl_free(type);
}
