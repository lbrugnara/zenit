
#include <fllib/Cstring.h>
#include "array.h"

ZenitArrayType* zenit_array_type_new(ZenitType *member_type)
{
    ZenitArrayType *type = fl_malloc(sizeof(ZenitArrayType));
    type->base.typekind = ZENIT_TYPE_ARRAY;
    type->member_type = member_type;

    return type;
}

unsigned long zenit_array_type_hash(ZenitArrayType *type)
{
    static const char *format = "[array][e:%lu]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, type->length, zenit_type_hash(type->member_type));

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

char* zenit_array_type_to_string(ZenitArrayType *type)
{
    if (type == NULL)
        return NULL;

    unsigned long type_hash = zenit_array_type_hash(type);

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

    fl_cstring_append(&string_value, zenit_type_to_string(type->member_type));

    // Update the string representation
    type->base.to_string.version = type_hash;
    type->base.to_string.value = fl_cstring_replace_realloc(type->base.to_string.value, type->base.to_string.value, string_value);

    fl_cstring_free(string_value);

    return type->base.to_string.value;
}

bool zenit_array_type_equals(ZenitArrayType *type_a, ZenitType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (ZenitType*) type_a == type_b;

    if (type_b->typekind != ZENIT_TYPE_ARRAY)
        return false;

    ZenitArrayType *type_b_array = (ZenitArrayType*) type_b;

    return type_a->length == type_b_array->length && zenit_type_equals(type_a->member_type, type_b_array->member_type);
}

bool zenit_array_type_is_assignable_from(ZenitArrayType *target_type, ZenitType *from_type)
{
    if (from_type->typekind != ZENIT_TYPE_ARRAY)
        return false;

    ZenitArrayType *array_from_type = (ZenitArrayType*) from_type;

    if (target_type->length != array_from_type->length)
        return false;

    // If rhs is an empty array, as long as the target type is not NONE, it is safe to assign
    if (target_type->length == 0 && array_from_type->member_type->typekind == ZENIT_TYPE_NONE)
        return target_type->member_type->typekind != ZENIT_TYPE_NONE;

    return zenit_type_is_assignable_from(target_type->member_type, array_from_type->member_type);
}

bool zenit_array_type_is_castable_to(ZenitArrayType *array_type, ZenitType *target_type)
{
    if (array_type == NULL || target_type == NULL)
        return false;

    // We can only cast can the array type to another compatible array type
    if (target_type->typekind != ZENIT_TYPE_ARRAY)
        return false;

    // If target_type is the same type of the array type, we are ok to cast    
    if (zenit_array_type_equals(array_type, target_type))
        return true;    

    // If we reached this point, it means that:
    //  - The target type has a different number of elements than the array type
    //  - The number of elements are equals, in that case we should check if we can cast from the array's declared/inferred
    //    type to the target declared/inferred type
    ZenitArrayType *target_array_type = (ZenitArrayType*) target_type;

    if (array_type->length != target_array_type->length)
        return false;

    return zenit_type_is_castable_to(array_type->member_type, target_array_type->member_type);
}

bool zenit_array_type_can_unify(ZenitArrayType *array_type, ZenitType *type_b)
{
    if (array_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE)
        return true;

    if (type_b->typekind != ZENIT_TYPE_ARRAY)
        return false;

    if (zenit_array_type_equals(array_type, type_b))
        return true;

    ZenitArrayType *arr_type_b = (ZenitArrayType*) type_b;

    if (array_type->length != arr_type_b->length)
        return false;

    if (!zenit_type_can_unify(array_type->member_type, arr_type_b->member_type))
        return false;

    return true;
}

void zenit_array_type_free(ZenitArrayType *type)
{
    if (!type)
        return;

    if (type->base.to_string.value != NULL)
        fl_cstring_free(type->base.to_string.value);

    fl_free(type);
}
