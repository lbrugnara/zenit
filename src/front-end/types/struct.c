
#include <stdlib.h>
#include <fllib/Cstring.h>
#include "struct.h"

static void member_free(void *ptr)
{
    if (!ptr)
        return;

    ZenitStructTypeMember *member = (ZenitStructTypeMember*) ptr;
    
    if (member->name)
        fl_cstring_free(member->name);

    fl_free(member);
}

ZenitStructType* zenit_type_struct_new(char *name)
{
    ZenitStructType *type = fl_malloc(sizeof(ZenitStructType));
    type->base.typekind = ZENIT_TYPE_STRUCT;
    type->name = name != NULL ? fl_cstring_dup(name) : NULL; // If NULL is an anonymous struct
    type->members = fl_list_new_args((struct FlListArgs) {
        .value_cleaner = member_free
    });

    return type;
}

void zenit_type_struct_add_member(ZenitStructType *struct_type, const char *name, ZenitType *member_type)
{
    ZenitStructTypeMember *member = fl_malloc(sizeof(ZenitStructTypeMember));

    member->name = fl_cstring_dup(name);
    member->type = member_type;

    fl_list_append(struct_type->members, member);
}

ZenitStructTypeMember* zenit_type_struct_get_member(ZenitStructType *struct_type, const char *name)
{
    struct FlListNode *tmp = fl_list_head(struct_type->members);
    while (tmp != NULL)
    {
        ZenitStructTypeMember *member = (ZenitStructTypeMember*) tmp->value;
        
        if (flm_cstring_equals(name, member->name))
            return member;

        tmp = tmp->next;
    }

    return NULL;
}

unsigned long zenit_type_struct_hash(ZenitStructType *type)
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
char* zenit_type_struct_to_string(ZenitStructType *type)
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
            ZenitStructTypeMember *member = (ZenitStructTypeMember*) tmp->value;
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

bool zenit_type_struct_structurally_equals(ZenitStructType *type_a, ZenitStructType *type_b)
{
    if (fl_list_length(type_a->members) != fl_list_length(type_b->members))
        return false;
    
    // If the length of the unnamed structs are not equals, we can't cast it
    if (fl_list_length(type_a->members) != fl_list_length(type_b->members))
        return false;

    struct FlListNode *src_node = fl_list_head(type_a->members);    

    while (src_node)
    {
        ZenitStructTypeMember *src_member = (ZenitStructTypeMember*) src_node->value;
        ZenitStructTypeMember *dest_member = zenit_type_struct_get_member(type_b, src_member->name);

        if (dest_member == NULL)
            return false;

        bool equals = false;
        if (src_member->type->typekind != ZENIT_TYPE_STRUCT || dest_member->type->typekind != ZENIT_TYPE_STRUCT)
            equals = zenit_type_equals(src_member->type, dest_member->type);
        else
            equals = zenit_type_struct_structurally_equals((ZenitStructType*) src_member->type, (ZenitStructType*) dest_member->type);
        
        if (!equals)
            return false;

        src_node = src_node->next;
    }

    return true;
}

bool zenit_type_struct_equals(ZenitStructType *type_a, ZenitType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (ZenitType*) type_a == type_b;

    if (type_b->typekind != ZENIT_TYPE_STRUCT)
        return false;

    ZenitStructType *type_b_struct = (ZenitStructType*) type_b;
    
    if (type_a->name != NULL && type_b_struct->name != NULL)
        return flm_cstring_equals(type_a->name, type_b_struct->name);
    else if (type_a->name != type_b_struct->name) // If one of them has a name, they are not equals
        return false;

    // Structural equality
    return zenit_type_struct_structurally_equals(type_a, type_b_struct);
}

bool zenit_type_struct_is_assignable_from(ZenitStructType *target_type, ZenitType *from_type)
{
    if (!target_type || !from_type)
        return false;

    if (from_type->typekind != ZENIT_TYPE_STRUCT)
        return false;

    ZenitStructType *struct_from_type = (ZenitStructType*) from_type;

    if (target_type->name != NULL && struct_from_type->name != NULL)
        return flm_cstring_equals(target_type->name, struct_from_type->name);

    // If the length of the unnamed structs are not equals, we can't cast it
    if (fl_list_length(target_type->members) != fl_list_length(struct_from_type->members))
        return false;

    struct FlListNode *src_node = fl_list_head(target_type->members);    

    while (src_node)
    {
        ZenitStructTypeMember *src_member = (ZenitStructTypeMember*) src_node->value;
        ZenitStructTypeMember *dest_member = zenit_type_struct_get_member(struct_from_type, src_member->name);

        if (dest_member == NULL)
            return false;

        if (!zenit_type_is_assignable_from(src_member->type, dest_member->type))
            return false;

        src_node = src_node->next;
    }

    return true;
}

bool zenit_type_struct_is_castable_to(ZenitStructType *struct_type, ZenitType *target_type)
{
    if (struct_type == NULL || target_type == NULL)
        return false;

    if (target_type->typekind != ZENIT_TYPE_STRUCT)
        return false;

    ZenitStructType *target_struct_type = (ZenitStructType*) target_type;

    // Dummy check but we can "cast" to the same type
    if (struct_type->name != NULL && target_struct_type->name != NULL)
        return flm_cstring_equals(struct_type->name, target_struct_type->name);

    // If the length of the unnamed structs are not equals, we can't cast it
    if (fl_list_length(struct_type->members) != fl_list_length(target_struct_type->members))
        return false;

    struct FlListNode *src_node = fl_list_head(struct_type->members);    

    while (src_node)
    {
        ZenitStructTypeMember *src_member = (ZenitStructTypeMember*) src_node->value;
        ZenitStructTypeMember *dest_member = zenit_type_struct_get_member(target_struct_type, src_member->name);

        if (dest_member == NULL)
            return false;

        if (!zenit_type_is_castable_to(src_member->type, dest_member->type))
            return false;

        src_node = src_node->next;
    }

    return true;
}

bool zenit_type_struct_can_unify(ZenitStructType *struct_type, ZenitType *type_b)
{
    if (struct_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE)
        return true;

    if (type_b->typekind != ZENIT_TYPE_STRUCT)
        return false;

    if (zenit_type_struct_equals(struct_type, type_b))
        return true;

    ZenitStructType *struct_type_b = (ZenitStructType*) type_b;

    if (struct_type->name != NULL && struct_type_b->name != NULL && flm_cstring_equals(struct_type->name, struct_type_b->name))
        return true;

    struct FlListNode *member_node = fl_list_head(struct_type->members);

    while (member_node)
    {
        ZenitStructTypeMember *member_a = (ZenitStructTypeMember*) member_node->value;
        ZenitStructTypeMember *member_b = zenit_type_struct_get_member(struct_type_b, member_a->name);

        if (member_b == NULL || !zenit_type_can_unify(member_a->type, member_b->type))
            return false;

        member_node = member_node->next; 
    }

    return true;
}

void zenit_type_struct_free(ZenitStructType *type)
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
