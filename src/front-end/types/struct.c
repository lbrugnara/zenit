#include <fllib.h>
#include <stdlib.h>
#include "struct.h"

struct ZenitStructTypeInfo* zenit_type_struct_new(enum ZenitTypeSource source, char *name)
{
    struct ZenitStructTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitStructTypeInfo));
    typeinfo->base.type = ZENIT_TYPE_STRUCT;
    typeinfo->base.source = source;
    typeinfo->name = fl_cstring_dup(name);
    // FIXME: Allocate members array
    return typeinfo;
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

    // FIXME: Once the members are implemented we need to copy them too
    return zenit_type_struct_new(src_type->base.source, src_type->name);
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

    char *string_value = fl_cstring_dup(typeinfo->name);
    
    // Update the string representation
    typeinfo->base.to_string.version = type_hash;
    typeinfo->base.to_string.value = fl_cstring_replace_realloc(typeinfo->base.to_string.value, typeinfo->base.to_string.value, string_value);

    fl_cstring_free(string_value);

    return typeinfo->base.to_string.value;
}

bool zenit_type_struct_equals(struct ZenitStructTypeInfo *type_a, struct ZenitTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZenitTypeInfo*) type_a == type_b;

    if (type_b->type != ZENIT_TYPE_STRUCT)
        return false;

    // FIXME: Once the members are implemented we need to copy them too
    struct ZenitStructTypeInfo *type_b_struct = (struct ZenitStructTypeInfo*) type_b;
    
    if (type_a->name == NULL || type_b_struct->name == NULL)
        return type_a->name == type_b_struct->name;

    return flm_cstring_equals(type_a->name, type_b_struct->name);
}

bool zenit_type_struct_is_assignable_from(struct ZenitStructTypeInfo *target_type, struct ZenitTypeInfo *from_type)
{
    if (!target_type || !from_type)
        return false;

    if (from_type->type != ZENIT_TYPE_STRUCT)
        return false;

    // FIXME: Once the members are implemented we need to copy them too
    struct ZenitStructTypeInfo *struct_from_type = (struct ZenitStructTypeInfo*) from_type;
    
    return flm_cstring_equals(target_type->name, struct_from_type->name);
}

bool zenit_type_struct_is_castable_to(struct ZenitStructTypeInfo *struct_type, struct ZenitTypeInfo *target_type)
{
    if (struct_type == NULL || target_type == NULL)
        return false;

    if (zenit_type_struct_equals(struct_type, target_type))
        return true;

    // FIXME: Once the members are implemented we need to check them here
    return false;
}

bool zenit_type_struct_unify(struct ZenitStructTypeInfo *struct_type, struct ZenitTypeInfo *type_b, struct ZenitTypeInfo **unified)
{
    if (struct_type == NULL || type_b == NULL)
        return false;

    if (type_b->type == ZENIT_TYPE_NONE)
    {
        if (unified)
        {
            *unified = (struct ZenitTypeInfo*) zenit_type_struct_copy(struct_type);
            (*unified)->source = ZENIT_TYPE_SRC_INFERRED;
        }
        return true;
    }

    if (type_b->type != ZENIT_TYPE_STRUCT)
        return false;

    if (zenit_type_struct_equals(struct_type, type_b))
    {
        if (unified)
        {
            *unified = (struct ZenitTypeInfo*) zenit_type_struct_copy(struct_type);
            (*unified)->source = ZENIT_TYPE_SRC_INFERRED;
        }
        return true;
    }

    struct ZenitStructTypeInfo *struct_type_b = (struct ZenitStructTypeInfo*) type_b;

    // FIXME: Once the members are implemented we need to check them too    
    if (!flm_cstring_equals(struct_type->name, struct_type_b->name))
        return false;

    if (unified)
    {
        *unified = (struct ZenitTypeInfo*) zenit_type_struct_new(ZENIT_TYPE_SRC_INFERRED, struct_type->name);
        // FIXME: Once the members are implemented we need to copy them too
    }

    return true;
}

void zenit_type_struct_free(struct ZenitStructTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->base.to_string.value != NULL)
        fl_cstring_free(typeinfo->base.to_string.value);

    if (typeinfo->name)
        fl_cstring_free(typeinfo->name);

    // FIXME: Clean members' memory once they are implemented

    fl_free(typeinfo);
}
