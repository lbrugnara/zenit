#include <fllib.h>
#include <stdlib.h>
#include "struct.h"

struct StructMember {
    const char *name;
    struct ZenitTypeInfo *type;
};

void member_free(void *ptr)
{
    if (!ptr)
        return;

    struct StructMember *member = (struct StructMember*) ptr;
    
    if (member->name)
        fl_cstring_free(member->name);
        
    if (member->type)
        zenit_type_free(member->type);

    fl_free(member);
}

struct ZenitStructTypeInfo* zenit_type_struct_new(enum ZenitTypeSource source, char *name)
{
    struct ZenitStructTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitStructTypeInfo));
    typeinfo->base.type = ZENIT_TYPE_STRUCT;
    typeinfo->base.source = source;
    typeinfo->base.sealed = false;
    typeinfo->name = name != NULL ? fl_cstring_dup(name) : NULL; // If NULL is an anonymous struct
    typeinfo->members = fl_list_new_args((struct FlListArgs) {
        .value_cleaner = member_free
    });

    return typeinfo;
}

void zenit_type_struct_add_member(struct ZenitStructTypeInfo *typeinfo, const char *name, struct ZenitTypeInfo *type)
{
    struct StructMember *member = fl_malloc(sizeof(struct StructMember));

    member->name = fl_cstring_dup(name);
    member->type = type;

    fl_list_append(typeinfo->members, member);
}

unsigned long zenit_type_struct_hash(struct ZenitStructTypeInfo *typeinfo)
{
    static const char *format = "[struct][n:%s]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, typeinfo->name);

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

struct ZenitStructTypeInfo* zenit_type_struct_copy(struct ZenitStructTypeInfo *src_type)
{
    if (!src_type)
        return NULL;

    struct ZenitStructTypeInfo *copy = zenit_type_struct_new(src_type->base.source, src_type->name);

    struct FlListNode *src_node = fl_list_head(src_type->members);

    while (src_node)
    {
        struct StructMember *src_member = (struct StructMember*) src_node->value;

        zenit_type_struct_add_member(copy, src_member->name, zenit_type_copy(src_member->type));

        src_node = src_node->next;
    }

    return copy;
}


/*
 * Function: zenit_type_to_string
 *  This function has the added complexity of taking into account if 
 *  the type is an array and its size, therefore we need to use a heap
 *  allocated string, but we benefit from the <type_string_mapping_pool> variable
 *  to reuse strings.
 */
char* zenit_type_struct_to_string(struct ZenitStructTypeInfo *typeinfo)
{
    if (typeinfo == NULL)
        return NULL;

    unsigned long type_hash = zenit_type_hash((struct ZenitTypeInfo*) typeinfo);

    if (typeinfo->base.to_string.value == NULL)
    {
        // First call, initialize the value
        typeinfo->base.to_string.value = fl_cstring_new(0);
    }
    else if (type_hash == typeinfo->base.to_string.version)
    {
        // If the type information didn't change, just return the 
        // current value
        return typeinfo->base.to_string.value;
    }

    char *string_value = NULL;
    
    if (typeinfo->name != NULL)
    {
        string_value = fl_cstring_dup(typeinfo->name);
    }
    else
    {
        string_value = fl_cstring_dup("{ ");

        struct FlListNode *tmp = fl_list_head(typeinfo->members);
        while (tmp != NULL)
        {
            struct StructMember *member = (struct StructMember*) tmp->value;
            fl_cstring_vappend(&string_value, "%s: %s", member->name, member->type ? zenit_type_to_string(member->type) : "<unknown>");

            tmp = tmp->next;

            if (tmp)
                fl_cstring_append(&string_value, ", ");
        }

        fl_cstring_append(&string_value, " }");
    }
    
    // Update the string representation
    typeinfo->base.to_string.version = type_hash;
    typeinfo->base.to_string.value = fl_cstring_replace_realloc(typeinfo->base.to_string.value, typeinfo->base.to_string.value, string_value);

    fl_cstring_free(string_value);

    return typeinfo->base.to_string.value;
}

static inline bool structurally_equals(struct ZenitStructTypeInfo *type_a, struct ZenitStructTypeInfo *type_b)
{
    if (fl_list_length(type_a->members) != fl_list_length(type_b->members))
        return false;

    struct FlListNode *a_node = fl_list_head(type_a->members);    

    while (a_node)
    {
        struct StructMember *a_member = (struct StructMember*) a_node->value;

        struct FlListNode *b_node = fl_list_head(type_b->members);
        bool a_exists_in_b = false;
        while (b_node)
        {
            struct StructMember *b_member = (struct StructMember*) b_node->value;

            if (flm_cstring_equals(a_member->name, b_member->name))
            {
                bool equals = false;
                if (a_member->type->type != ZENIT_TYPE_STRUCT || b_member->type->type != ZENIT_TYPE_STRUCT)
                    equals = zenit_type_equals(a_member->type, b_member->type);
                else
                    equals = structurally_equals((struct ZenitStructTypeInfo*) a_member->type, (struct ZenitStructTypeInfo*) b_member->type);
                
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

bool zenit_type_struct_equals(struct ZenitStructTypeInfo *type_a, struct ZenitTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZenitTypeInfo*) type_a == type_b;

    if (type_b->type != ZENIT_TYPE_STRUCT)
        return false;

    struct ZenitStructTypeInfo *type_b_struct = (struct ZenitStructTypeInfo*) type_b;
    
    if (type_a->name != NULL && type_b_struct->name != NULL)
        return flm_cstring_equals(type_a->name, type_b_struct->name);
    else if (type_a->name != type_b_struct->name) // If one of them has a name, they are not equals
        return false;

    // Structural equality
    return structurally_equals(type_a, type_b_struct);
}

bool zenit_type_struct_is_assignable_from(struct ZenitStructTypeInfo *target_type, struct ZenitTypeInfo *from_type)
{
    if (!target_type || !from_type)
        return false;

    if (from_type->type != ZENIT_TYPE_STRUCT)
        return false;

    struct ZenitStructTypeInfo *struct_from_type = (struct ZenitStructTypeInfo*) from_type;

    if (target_type->name != NULL && struct_from_type->name != NULL)
        return flm_cstring_equals(target_type->name, struct_from_type->name);

    // Structural equality (we can safely convert between struturally equals objects)
    return structurally_equals(target_type, struct_from_type);
}

bool zenit_type_struct_is_castable_to(struct ZenitStructTypeInfo *struct_type, struct ZenitTypeInfo *target_type)
{
    if (struct_type == NULL || target_type == NULL)
        return false;

    if (target_type->type != ZENIT_TYPE_STRUCT)
        return false;

    // If we can assign the struct to a target type instance, we can cast to it too
    return zenit_type_is_assignable_from(target_type, (struct ZenitTypeInfo*) struct_type);
}

struct ZenitTypeInfo* zenit_type_struct_unify(struct ZenitStructTypeInfo *struct_type, struct ZenitTypeInfo *type_b)
{
    if (struct_type == NULL || type_b == NULL)
        return NULL;

    if (type_b->type == ZENIT_TYPE_NONE)
    {
        struct ZenitTypeInfo *unified = (struct ZenitTypeInfo*) zenit_type_struct_copy(struct_type);
        unified->source = ZENIT_TYPE_SRC_INFERRED;
        return unified;
    }

    if (type_b->type != ZENIT_TYPE_STRUCT)
        return NULL;

    if (zenit_type_struct_equals(struct_type, type_b))
    {
        struct ZenitTypeInfo *unified = (struct ZenitTypeInfo*) zenit_type_struct_copy(struct_type);
        unified->source = ZENIT_TYPE_SRC_INFERRED;
        return unified;
    }

    struct ZenitStructTypeInfo *struct_type_b = (struct ZenitStructTypeInfo*) type_b;

    // If they are structurally equals, we just copy one of them and return an unnamed struct
    // (an unnamed struct can be casted or assigned to a named struct as long as they are structurally
    // equals and the other way around)
    if (structurally_equals(struct_type, struct_type_b))
        return (struct ZenitTypeInfo*) zenit_type_struct_new(ZENIT_TYPE_SRC_INFERRED, NULL);

    // FIXME: Let's assume we can't unify structurally inequal structs
    return NULL;
}

bool zenit_type_struct_can_unify(struct ZenitStructTypeInfo *struct_type, struct ZenitTypeInfo *type_b)
{
    if (struct_type == NULL || type_b == NULL)
        return false;

    if (type_b->type == ZENIT_TYPE_NONE)
        return true;

    if (type_b->type != ZENIT_TYPE_STRUCT)
        return false;

    if (zenit_type_struct_equals(struct_type, type_b))
        return true;

    struct ZenitStructTypeInfo *struct_type_b = (struct ZenitStructTypeInfo*) type_b;

    if (struct_type->name != NULL && struct_type_b->name != NULL && flm_cstring_equals(struct_type->name, struct_type_b->name))
        return true;

    // FIXME: Let's assume we can't unify structurally inequal structs
    return false;
}

void zenit_type_struct_free(struct ZenitStructTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->base.to_string.value != NULL)
        fl_cstring_free(typeinfo->base.to_string.value);

    if (typeinfo->name)
        fl_cstring_free(typeinfo->name);

    if (typeinfo->members)
        fl_list_free(typeinfo->members);

    fl_free(typeinfo);
}
