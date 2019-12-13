#include <fllib.h>
#include "array.h"

struct ZenitArrayTypeInfo* zenit_type_array_new(enum ZenitTypeSource source, struct ZenitTypeInfo *member_type)
{
    struct ZenitArrayTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitArrayTypeInfo));
    typeinfo->base.type = ZENIT_TYPE_ARRAY;
    typeinfo->base.source = source;
    typeinfo->members = fl_array_new(sizeof(struct ZenitArrayTypeInfo*), 0);
    typeinfo->member_type = member_type;

    return typeinfo;
}

void zenit_type_array_add_member(struct ZenitArrayTypeInfo *typeinfo, struct ZenitTypeInfo *element)
{
    // Do the same for the type information
    typeinfo->members = fl_array_append(typeinfo->members, &element);
}

unsigned long zenit_type_array_hash(struct ZenitArrayTypeInfo *typeinfo)
{
    static const char *format = "[array][e:%lu]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, typeinfo->length, zenit_type_hash(typeinfo->member_type));

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

struct ZenitArrayTypeInfo* zenit_type_array_copy(struct ZenitArrayTypeInfo *source)
{
    if (!source)
        return NULL;

    struct ZenitArrayTypeInfo *dest = zenit_type_array_new(source->base.source, zenit_type_copy(source->member_type));
    dest->length = source->length;

    for (size_t i=0; i < fl_array_length(source->members); i++)
        zenit_type_array_add_member(dest, zenit_type_copy(source->members[i]));
    
    return dest;
}

char* zenit_type_array_to_string(struct ZenitArrayTypeInfo *typeinfo)
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

    // We allocate memory for the string representation of this <struct ZenitTypeInfo> object
    char *string_value = fl_cstring_new(0);
    
    struct ZenitArrayTypeInfo *array_type = (struct ZenitArrayTypeInfo*) typeinfo;

    fl_cstring_append(&string_value, "[");
    fl_cstring_vappend(&string_value, "%zu", array_type->length);
    fl_cstring_append(&string_value, "]");

    fl_cstring_append(&string_value, zenit_type_to_string(array_type->member_type));

    // Update the string representation
    typeinfo->base.to_string.version = type_hash;
    typeinfo->base.to_string.value = fl_cstring_replace_realloc(typeinfo->base.to_string.value, typeinfo->base.to_string.value, string_value);

    fl_cstring_free(string_value);

    return typeinfo->base.to_string.value;
}

bool zenit_type_array_equals(struct ZenitArrayTypeInfo *type_a, struct ZenitTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZenitTypeInfo*) type_a == type_b;

    if (type_b->type != ZENIT_TYPE_ARRAY)
        return false;

    struct ZenitArrayTypeInfo *type_b_array = (struct ZenitArrayTypeInfo*) type_b;

    return type_a->length == type_b_array->length && zenit_type_equals(type_a->member_type, type_b_array->member_type);
}

bool zenit_type_array_is_assignable_from(struct ZenitArrayTypeInfo *target_type, struct ZenitTypeInfo *from_type)
{
    if (from_type->type != ZENIT_TYPE_ARRAY)
        return false;

    struct ZenitArrayTypeInfo *array_from_type = (struct ZenitArrayTypeInfo*) from_type;

    if (target_type->length != array_from_type->length)
        return false;

    // If rhs is an empty array, as long as the target type is not NONE, it is safe to assign
    if (target_type->length == 0 && array_from_type->member_type->type == ZENIT_TYPE_NONE)
        return target_type->member_type->type != ZENIT_TYPE_NONE;

    return zenit_type_is_assignable_from(target_type->member_type, array_from_type->member_type);
}

bool zenit_type_array_is_castable_to(struct ZenitArrayTypeInfo *array_type, struct ZenitTypeInfo *target_type)
{
    if (array_type == NULL || target_type == NULL)
        return false;

    // We can only cast can the array type to another compatible array type
    if (target_type->type != ZENIT_TYPE_ARRAY)
        return false;

    // If target_type is the same type of the array type, we are ok to cast    
    if (zenit_type_array_equals(array_type, target_type))
        return true;    

    // If we reached this point, it means that:
    //  - The target type has a different number of elements than the array type
    //  - The number of elements are equals, in that case we should check if we can cast from the array's declared/inferred
    //    type to the target declared/inferred type
    struct ZenitArrayTypeInfo *target_array_type = (struct ZenitArrayTypeInfo*) target_type;

    if (array_type->length != target_array_type->length)
        return false;

    return zenit_type_is_castable_to(array_type->member_type, target_array_type->member_type);
}

struct ZenitTypeInfo* zenit_type_array_unify(struct ZenitArrayTypeInfo *array_type, struct ZenitTypeInfo *type_b)
{
    if (array_type == NULL || type_b == NULL)
        return NULL;

    if (type_b->type == ZENIT_TYPE_NONE)
    {
        struct ZenitTypeInfo *unified = (struct ZenitTypeInfo*) zenit_type_array_copy(array_type);
        unified->source = ZENIT_TYPE_SRC_INFERRED;
        return unified;
    }

    if (type_b->type != ZENIT_TYPE_ARRAY)
        return NULL;

    if (zenit_type_array_equals(array_type, type_b))
    {
        struct ZenitTypeInfo *unified = (struct ZenitTypeInfo*) zenit_type_array_copy(array_type);
        unified->source = ZENIT_TYPE_SRC_INFERRED;
        return unified;
    }

    struct ZenitArrayTypeInfo *arr_type_b = (struct ZenitArrayTypeInfo*) type_b;

    if (array_type->length != arr_type_b->length)
        return NULL;

    if (!zenit_type_can_unify(array_type->member_type, arr_type_b->member_type))
        return NULL;
    
    struct ZenitTypeInfo *unified_member_type = zenit_type_unify(array_type->member_type, arr_type_b->member_type);

    bool can_unify = true;
    for (size_t i=0; i < fl_array_length(array_type->members); i++)
        can_unify = can_unify && zenit_type_can_unify(unified_member_type, array_type->members[i]);

    if (can_unify)
    {
        struct ZenitArrayTypeInfo *unified_array_type = zenit_type_array_new(ZENIT_TYPE_SRC_INFERRED, unified_member_type);
        unified_array_type->length = array_type->length;
        for (size_t i=0; i < fl_array_length(array_type->members); i++)
        {
            struct ZenitTypeInfo *unified_type = zenit_type_unify(unified_member_type, array_type->members[i]);
            zenit_type_array_add_member(unified_array_type, unified_type);
        }

        return (struct ZenitTypeInfo*) unified_array_type;
    }
    
    zenit_type_free(unified_member_type);
    return NULL;
}

bool zenit_type_array_can_unify(struct ZenitArrayTypeInfo *array_type, struct ZenitTypeInfo *type_b)
{
    if (array_type == NULL || type_b == NULL)
        return false;

    if (type_b->type == ZENIT_TYPE_NONE)
        return true;

    if (type_b->type != ZENIT_TYPE_ARRAY)
        return false;

    if (zenit_type_array_equals(array_type, type_b))
        return true;

    struct ZenitArrayTypeInfo *arr_type_b = (struct ZenitArrayTypeInfo*) type_b;

    if (array_type->length != arr_type_b->length)
        return false;

    if (!zenit_type_can_unify(array_type->member_type, arr_type_b->member_type))
        return false;

    for (size_t i=0; i < fl_array_length(array_type->members); i++)
        if (!zenit_type_can_unify(array_type->members[i], arr_type_b->members[i]))
            return false;

    return true;
}

void zenit_type_array_free(struct ZenitArrayTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->base.to_string.value != NULL)
        fl_cstring_free(typeinfo->base.to_string.value);

    zenit_type_free(typeinfo->member_type);

    fl_array_free_each_pointer(typeinfo->members, (FlArrayFreeElementFunc) zenit_type_free);

    fl_free(typeinfo);
}
