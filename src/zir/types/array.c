
#include <stdio.h>
#include <string.h>
#include <fllib/Mem.h>
#include <fllib/Cstring.h>
#include "array.h"

ZirArrayType* zir_array_type_new(ZirType *member_type)
{
    ZirArrayType *type = fl_malloc(sizeof(ZirArrayType));
    type->base.typekind = ZIR_TYPE_ARRAY;
    type->member_type = member_type;

    return type;
}

unsigned long zir_array_type_hash(ZirArrayType *type)
{
    static const char *format = "[array][e:%lu]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, type->length, zir_type_hash(type->member_type));

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

char* zir_array_type_to_string(ZirArrayType *type)
{
    if (type == NULL)
        return NULL;

    unsigned long type_hash = zir_array_type_hash(type);

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

    // We allocate memory for the string representation of this object
    char *string_value = fl_cstring_new(0);

    fl_cstring_append(&string_value, "[");
    fl_cstring_vappend(&string_value, "%zu", type->length);
    fl_cstring_append(&string_value, "]");

    fl_cstring_append(&string_value, zir_type_to_string(type->member_type));

    // Update the string representation
    type->base.to_string.version = type_hash;
    type->base.to_string.value = fl_cstring_replace_realloc(type->base.to_string.value, type->base.to_string.value, string_value);

    fl_cstring_free(string_value);

    return type->base.to_string.value;
}

bool zir_array_type_equals(ZirArrayType *type_a, ZirType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (ZirType*) type_a == type_b;

    if (type_b->typekind != ZIR_TYPE_ARRAY)
        return false;

    ZirArrayType *type_b_array = (ZirArrayType*) type_b;

    return type_a->length == type_b_array->length && zir_type_equals(type_a->member_type, type_b_array->member_type);
}

bool zir_array_type_is_assignable_from(ZirArrayType *target_type, ZirType *from_type)
{
    if (from_type->typekind != ZIR_TYPE_ARRAY)
        return false;

    ZirArrayType *array_from_type = (ZirArrayType*) from_type;

    if (target_type->length != array_from_type->length)
        return false;

    // If rhs is an empty array, as long as the target type is not NONE, it is safe to assign
    if (target_type->length == 0 && array_from_type->member_type->typekind == ZIR_TYPE_NONE)
        return target_type->member_type->typekind != ZIR_TYPE_NONE;

    return zir_type_is_assignable_from(target_type->member_type, array_from_type->member_type);
}

bool zir_array_type_is_castable_to(ZirArrayType *array_type, ZirType *target_type)
{
    if (array_type == NULL || target_type == NULL)
        return false;

    // We can only cast can the array type to another compatible array type
    if (target_type->typekind != ZIR_TYPE_ARRAY)
        return false;

    // If target_type is the same type of the array type, we are ok to cast    
    if (zir_array_type_equals(array_type, target_type))
        return true;    

    // If we reached this point, it means that:
    //  - The target type has a different number of elements than the array type
    //  - The number of elements are equals, in that case we should check if we can cast from the array's declared/inferred
    //    type to the target declared/inferred type
    ZirArrayType *target_array_type = (ZirArrayType*) target_type;

    if (array_type->length != target_array_type->length)
        return false;

    return zir_type_is_castable_to(array_type->member_type, target_array_type->member_type);
}

size_t zir_array_type_size(ZirArrayType *type)
{
    if (!type)
        return 0;

    return type->length * zir_type_size(type->member_type);
}

void zir_array_type_free(ZirArrayType *type)
{
    if (!type)
        return;

    if (type->base.to_string.value != NULL)
        fl_cstring_free(type->base.to_string.value);

    zir_type_free(type->member_type);

    fl_free(type);
}
