#include <fllib.h>
#include <stdlib.h>
#include "struct.h"

struct ZirStructType* zir_type_struct_new(char *name)
{
    struct ZirStructType *type = fl_malloc(sizeof(struct ZirStructType));
    type->base.typekind = ZIR_TYPE_STRUCT;
    type->name = fl_cstring_dup(name);
    // FIXME: Allocate members array
    return type;
}

unsigned long zir_type_struct_hash(struct ZirStructType *type)
{
    static const char *format = "[struct][n:%s][m:%zu]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, type->name, type->members ? fl_array_length(type->members) : 0);

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

struct ZirStructType* zir_type_struct_copy(struct ZirStructType *src_type)
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
char* zir_type_struct_to_string(struct ZirStructType *type)
{
    if (type == NULL)
        return NULL;

    unsigned long type_hash = zir_type_hash((struct ZirType*) type);

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

    char *string_value = fl_cstring_dup(type->name);
    
    // Update the string representation
    type->base.to_string.version = type_hash;
    type->base.to_string.value = fl_cstring_replace_realloc(type->base.to_string.value, type->base.to_string.value, string_value);

    fl_cstring_free(string_value);

    return type->base.to_string.value;
}

bool zir_type_struct_equals(struct ZirStructType *type_a, struct ZirType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZirType*) type_a == type_b;

    if (type_b->typekind != ZIR_TYPE_STRUCT)
        return false;

    // FIXME: Once the members are implemented we need to copy them too
    struct ZirStructType *type_b_struct = (struct ZirStructType*) type_b;
    
    if (type_a->name == NULL || type_b_struct->name == NULL)
        return type_a->name == type_b_struct->name;

    return flm_cstring_equals(type_a->name, type_b_struct->name);
}

bool zir_type_struct_is_assignable_from(struct ZirStructType *target_type, struct ZirType *from_type)
{
    if (!target_type || !from_type)
        return false;

    if (from_type->typekind != ZIR_TYPE_STRUCT)
        return false;

    // FIXME: Once the members are implemented we need to copy them too
    struct ZirStructType *struct_from_type = (struct ZirStructType*) from_type;
    
    return flm_cstring_equals(target_type->name, struct_from_type->name);
}

bool zir_type_struct_is_castable_to(struct ZirStructType *struct_type, struct ZirType *target_type)
{
    if (struct_type == NULL || target_type == NULL)
        return false;

    if (zir_type_struct_equals(struct_type, target_type))
        return true;

    // FIXME: Once the members are implemented we need to check them here
    return false;
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

    // FIXME: Free the members once they are implemented

    fl_free(type);
}
