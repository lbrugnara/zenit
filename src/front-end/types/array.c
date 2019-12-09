#include <fllib.h>
#include "array.h"

struct ZenitArrayTypeInfo* zenit_type_array_new(void)
{
    struct ZenitArrayTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitArrayTypeInfo));
    typeinfo->base.type = ZENIT_TYPE_ARRAY;
    typeinfo->members = fl_array_new(sizeof(struct ZenitArrayTypeInfo*), 0);

    return typeinfo;
}

void zenit_type_array_add_member(struct ZenitArrayTypeInfo *typeinfo, struct ZenitTypeInfo *element)
{
    // Do the same for the type information
    typeinfo->members = fl_array_append(typeinfo->members, &element);
}

unsigned long zenit_type_array_hash(struct ZenitArrayTypeInfo *typeinfo)
{
    static const char *format = "[array][n:%zu][e:%lu]";

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

    struct ZenitArrayTypeInfo *dest = zenit_type_array_new();

    dest->base.type = source->base.type;
    dest->length = source->length;
    dest->member_type = zenit_type_copy(source->member_type);
    dest->source = source->source;

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
    char tmp_string[20];
    snprintf(tmp_string, 20, "%zu", array_type->length);
    fl_cstring_append(&string_value, tmp_string);
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

void zenit_type_array_free(struct ZenitArrayTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->base.to_string.value != NULL)
        fl_cstring_free(typeinfo->base.to_string.value);

    fl_array_free(typeinfo->members);

    fl_free(typeinfo);
}
