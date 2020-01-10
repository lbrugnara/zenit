#include <fllib.h>
#include <stdlib.h>
#include "struct.h"

struct ZirStructTypeInfo* zir_type_struct_new(char *name)
{
    struct ZirStructTypeInfo *typeinfo = fl_malloc(sizeof(struct ZirStructTypeInfo));
    typeinfo->base.type = ZIR_TYPE_STRUCT;
    typeinfo->name = fl_cstring_dup(name);
    // FIXME: Allocate members array
    return typeinfo;
}

unsigned long zir_type_struct_hash(struct ZirStructTypeInfo *typeinfo)
{
    static const char *format = "[struct][n:%s][m:%zu]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, typeinfo->name, typeinfo->members ? fl_array_length(typeinfo->members) : 0);

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

struct ZirStructTypeInfo* zir_type_struct_copy(struct ZirStructTypeInfo *src_type)
{
    if (!src_type)
        return NULL;

    // FIXME: Once the members are implemented we need to copy them too
    return zir_type_struct_new(src_type->name);
}


/*
 * Function: zir_type_to_string
 *  This function has the added complexity of taking into account if 
 *  the type is an array and its size, therefore we need to use a heap
 *  allocated string, but we benefit from the <type_string_mapping_pool> variable
 *  to reuse strings.
 */
char* zir_type_struct_to_string(struct ZirStructTypeInfo *typeinfo)
{
    if (typeinfo == NULL)
        return NULL;

    unsigned long type_hash = zir_type_hash((struct ZirTypeInfo*) typeinfo);

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

bool zir_type_struct_equals(struct ZirStructTypeInfo *type_a, struct ZirTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZirTypeInfo*) type_a == type_b;

    if (type_b->type != ZIR_TYPE_STRUCT)
        return false;

    // FIXME: Once the members are implemented we need to copy them too
    struct ZirStructTypeInfo *type_b_struct = (struct ZirStructTypeInfo*) type_b;
    
    if (type_a->name == NULL || type_b_struct->name == NULL)
        return type_a->name == type_b_struct->name;

    return flm_cstring_equals(type_a->name, type_b_struct->name);
}

bool zir_type_struct_is_assignable_from(struct ZirStructTypeInfo *target_type, struct ZirTypeInfo *from_type)
{
    if (!target_type || !from_type)
        return false;

    if (from_type->type != ZIR_TYPE_STRUCT)
        return false;

    // FIXME: Once the members are implemented we need to copy them too
    struct ZirStructTypeInfo *struct_from_type = (struct ZirStructTypeInfo*) from_type;
    
    return flm_cstring_equals(target_type->name, struct_from_type->name);
}

bool zir_type_struct_is_castable_to(struct ZirStructTypeInfo *struct_type, struct ZirTypeInfo *target_type)
{
    if (struct_type == NULL || target_type == NULL)
        return false;

    if (zir_type_struct_equals(struct_type, target_type))
        return true;

    // FIXME: Once the members are implemented we need to check them here
    return false;
}

size_t zir_type_struct_size(struct ZirStructTypeInfo *typeinfo)
{
    if (!typeinfo)
        return 0;

    // FIXME: Add the size of the members
    return 0;
}

void zir_type_struct_free(struct ZirStructTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->base.to_string.value != NULL)
        fl_cstring_free(typeinfo->base.to_string.value);

    if (typeinfo->name)
        fl_cstring_free(typeinfo->name);

    // FIXME: Free the members once they are implemented

    fl_free(typeinfo);
}
