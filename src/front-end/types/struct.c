#include <fllib.h>
#include <stdlib.h>
#include "struct.h"

static void member_free(void *ptr)
{
    if (!ptr)
        return;

    struct ZenitStructTypeMember *member = (struct ZenitStructTypeMember*) ptr;
    
    if (member->name)
        fl_cstring_free(member->name);

    fl_free(member);
}

struct ZenitStructType* zenit_type_struct_new(char *name)
{
    struct ZenitStructType *type = fl_malloc(sizeof(struct ZenitStructType));
    type->base.typekind = ZENIT_TYPE_STRUCT;
    type->name = name != NULL ? fl_cstring_dup(name) : NULL; // If NULL is an anonymous struct
    type->members = fl_list_new_args((struct FlListArgs) {
        .value_cleaner = member_free
    });

    return type;
}

void zenit_type_struct_add_member(struct ZenitStructType *struct_type, const char *name, struct ZenitType *member_type)
{
    struct ZenitStructTypeMember *member = fl_malloc(sizeof(struct ZenitStructTypeMember));

    member->name = fl_cstring_dup(name);
    member->type = member_type;

    fl_list_append(struct_type->members, member);
}

struct ZenitStructTypeMember* zenit_type_struct_get_member(struct ZenitStructType *struct_type, const char *name)
{
    struct FlListNode *tmp = fl_list_head(struct_type->members);
    while (tmp != NULL)
    {
        struct ZenitStructTypeMember *member = (struct ZenitStructTypeMember*) tmp->value;
        
        if (flm_cstring_equals(name, member->name))
            return member;

        tmp = tmp->next;
    }

    return NULL;
}

unsigned long zenit_type_struct_hash(struct ZenitStructType *type)
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
 * Function: zenit_type_to_string
 *  This function has the added complexity of taking into account if 
 *  the type is an array and its size, therefore we need to use a heap
 *  allocated string, but we benefit from the <type_string_mapping_pool> variable
 *  to reuse strings.
 */
char* zenit_type_struct_to_string(struct ZenitStructType *type)
{
    if (type == NULL)
        return NULL;

    unsigned long type_hash = zenit_type_struct_hash(type);

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
            struct ZenitStructTypeMember *member = (struct ZenitStructTypeMember*) tmp->value;
            fl_cstring_vappend(&string_value, "%s: %s", member->name, member->type ? zenit_type_to_string(member->type) : "<unknown>");

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

bool zenit_type_struct_structurally_equals(struct ZenitStructType *type_a, struct ZenitStructType *type_b)
{
    if (fl_list_length(type_a->members) != fl_list_length(type_b->members))
        return false;

    struct FlListNode *a_node = fl_list_head(type_a->members);    

    while (a_node)
    {
        struct ZenitStructTypeMember *a_member = (struct ZenitStructTypeMember*) a_node->value;

        struct FlListNode *b_node = fl_list_head(type_b->members);
        bool a_exists_in_b = false;
        while (b_node)
        {
            struct ZenitStructTypeMember *b_member = (struct ZenitStructTypeMember*) b_node->value;

            if (flm_cstring_equals(a_member->name, b_member->name))
            {
                bool equals = false;
                if (a_member->type->typekind != ZENIT_TYPE_STRUCT || b_member->type->typekind != ZENIT_TYPE_STRUCT)
                    equals = zenit_type_equals(a_member->type, b_member->type);
                else
                    equals = zenit_type_struct_structurally_equals((struct ZenitStructType*) a_member->type, (struct ZenitStructType*) b_member->type);
                
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

bool zenit_type_struct_equals(struct ZenitStructType *type_a, struct ZenitType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZenitType*) type_a == type_b;

    if (type_b->typekind != ZENIT_TYPE_STRUCT)
        return false;

    struct ZenitStructType *type_b_struct = (struct ZenitStructType*) type_b;
    
    if (type_a->name != NULL && type_b_struct->name != NULL)
        return flm_cstring_equals(type_a->name, type_b_struct->name);
    else if (type_a->name != type_b_struct->name) // If one of them has a name, they are not equals
        return false;

    // Structural equality
    return zenit_type_struct_structurally_equals(type_a, type_b_struct);
}

bool zenit_type_struct_is_assignable_from(struct ZenitStructType *target_type, struct ZenitType *from_type)
{
    if (!target_type || !from_type)
        return false;

    if (from_type->typekind != ZENIT_TYPE_STRUCT)
        return false;

    struct ZenitStructType *struct_from_type = (struct ZenitStructType*) from_type;

    if (target_type->name != NULL && struct_from_type->name != NULL)
        return flm_cstring_equals(target_type->name, struct_from_type->name);

    // Structural equality (we can safely convert between struturally equals objects)
    return zenit_type_struct_structurally_equals(target_type, struct_from_type);
}

bool zenit_type_struct_is_castable_to(struct ZenitStructType *struct_type, struct ZenitType *target_type)
{
    if (struct_type == NULL || target_type == NULL)
        return false;

    if (target_type->typekind != ZENIT_TYPE_STRUCT)
        return false;

    // If we can assign the struct to a target type instance, we can cast to it too
    return zenit_type_is_assignable_from(target_type, (struct ZenitType*) struct_type);
}

bool zenit_type_struct_can_unify(struct ZenitStructType *struct_type, struct ZenitType *type_b)
{
    if (struct_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE)
        return true;

    if (type_b->typekind != ZENIT_TYPE_STRUCT)
        return false;

    if (zenit_type_struct_equals(struct_type, type_b))
        return true;

    struct ZenitStructType *struct_type_b = (struct ZenitStructType*) type_b;

    if (struct_type->name != NULL && struct_type_b->name != NULL && flm_cstring_equals(struct_type->name, struct_type_b->name))
        return true;

    struct FlListNode *member_node = fl_list_head(struct_type->members);

    while (member_node)
    {
        struct ZenitStructTypeMember *member_a = (struct ZenitStructTypeMember*) member_node->value;
        struct ZenitStructTypeMember *member_b = zenit_type_struct_get_member(struct_type_b, member_a->name);

        if (member_b == NULL || !zenit_type_can_unify(member_a->type, member_b->type))
            return false;

        member_node = member_node->next; 
    }

    return true;
}

void zenit_type_struct_free(struct ZenitStructType *type)
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
