#include <fllib.h>
#include "array.h"
#include "typeinfo.h"

struct ZenitArrayType* zenit_type_array_new(struct ZenitTypeInfo *member_type)
{
    struct ZenitArrayType *type = fl_malloc(sizeof(struct ZenitArrayType));
    type->base.typekind = ZENIT_TYPE_ARRAY;
    type->member_type = member_type;

    return type;
}

unsigned long zenit_type_array_hash(struct ZenitArrayType *type)
{
    static const char *format = "[array][e:%lu]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, type->length, zenit_type_hash(type->member_type->type));

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

char* zenit_type_array_to_string(struct ZenitArrayType *type)
{
    if (type == NULL)
        return NULL;

    unsigned long type_hash = zenit_type_array_hash(type);

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

    // We allocate memory for the string representation of this <struct ZenitTypeInfo> object
    char *string_value = fl_cstring_new(0);

    fl_cstring_append(&string_value, "[");
    fl_cstring_vappend(&string_value, "%zu", type->length);
    fl_cstring_append(&string_value, "]");

    fl_cstring_append(&string_value, zenit_type_to_string(type->member_type->type));

    // Update the string representation
    type->base.to_string.version = type_hash;
    type->base.to_string.value = fl_cstring_replace_realloc(type->base.to_string.value, type->base.to_string.value, string_value);

    fl_cstring_free(string_value);

    return type->base.to_string.value;
}

bool zenit_type_array_equals(struct ZenitArrayType *type_a, struct ZenitType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZenitType*) type_a == type_b;

    if (type_b->typekind != ZENIT_TYPE_ARRAY)
        return false;

    struct ZenitArrayType *type_b_array = (struct ZenitArrayType*) type_b;

    return type_a->length == type_b_array->length && zenit_type_equals(type_a->member_type->type, type_b_array->member_type->type);
}

bool zenit_type_array_is_assignable_from(struct ZenitArrayType *target_type, struct ZenitType *from_type)
{
    if (from_type->typekind != ZENIT_TYPE_ARRAY)
        return false;

    struct ZenitArrayType *array_from_type = (struct ZenitArrayType*) from_type;

    if (target_type->length != array_from_type->length)
        return false;

    // If rhs is an empty array, as long as the target type is not NONE, it is safe to assign
    if (target_type->length == 0 && array_from_type->member_type->type->typekind == ZENIT_TYPE_NONE)
        return target_type->member_type->type->typekind != ZENIT_TYPE_NONE;

    return zenit_type_is_assignable_from(target_type->member_type->type, array_from_type->member_type->type);
}

bool zenit_type_array_is_castable_to(struct ZenitArrayType *array_type, struct ZenitType *target_type)
{
    if (array_type == NULL || target_type == NULL)
        return false;

    // We can only cast can the array type to another compatible array type
    if (target_type->typekind != ZENIT_TYPE_ARRAY)
        return false;

    // If target_type is the same type of the array type, we are ok to cast    
    if (zenit_type_array_equals(array_type, target_type))
        return true;    

    // If we reached this point, it means that:
    //  - The target type has a different number of elements than the array type
    //  - The number of elements are equals, in that case we should check if we can cast from the array's declared/inferred
    //    type to the target declared/inferred type
    struct ZenitArrayType *target_array_type = (struct ZenitArrayType*) target_type;

    if (array_type->length != target_array_type->length)
        return false;

    return zenit_type_is_castable_to(array_type->member_type->type, target_array_type->member_type->type);
}

bool zenit_type_array_can_unify(struct ZenitArrayType *array_type, struct ZenitType *type_b)
{
    if (array_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE)
        return true;

    if (type_b->typekind != ZENIT_TYPE_ARRAY)
        return false;

    if (zenit_type_array_equals(array_type, type_b))
        return true;

    struct ZenitArrayType *arr_type_b = (struct ZenitArrayType*) type_b;

    if (array_type->length != arr_type_b->length)
        return false;

    if (!zenit_type_can_unify(array_type->member_type->type, arr_type_b->member_type->type))
        return false;

    return true;
}

void zenit_type_array_free(struct ZenitArrayType *type)
{
    if (!type)
        return;

    if (type->base.to_string.value != NULL)
        fl_cstring_free(type->base.to_string.value);

    zenit_typeinfo_free(type->member_type);

    fl_free(type);
}
