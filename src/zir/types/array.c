#include <fllib.h>
#include "array.h"

struct ZirArrayTypeInfo* zir_type_array_new(void)
{
    struct ZirArrayTypeInfo *typeinfo = fl_malloc(sizeof(struct ZirArrayTypeInfo));
    typeinfo->base.type = ZIR_TYPE_ARRAY;
    typeinfo->members = fl_array_new(sizeof(struct ZirArrayTypeInfo*), 0);

    return typeinfo;
}

void zir_type_array_add_member(struct ZirArrayTypeInfo *typeinfo, struct ZirTypeInfo *element)
{
    // Do the same for the type information
    typeinfo->members = fl_array_append(typeinfo->members, &element);
}

unsigned long zir_type_array_hash(struct ZirArrayTypeInfo *typeinfo)
{
    static const char *format = "[array][n:%zu][e:%lu]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, typeinfo->length, zir_type_hash(typeinfo->member_type));

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

struct ZirArrayTypeInfo* zir_type_array_copy(struct ZirArrayTypeInfo *source)
{
    if (!source)
        return NULL;

    struct ZirArrayTypeInfo *dest = zir_type_array_new();

    dest->base.type = source->base.type;
    dest->length = source->length;
    dest->member_type = zir_type_copy(source->member_type);
    dest->source = source->source;

    for (size_t i=0; i < fl_array_length(source->members); i++)
        zir_type_array_add_member(dest, zir_type_copy(source->members[i]));
    
    return dest;
}

char* zir_type_array_to_string(struct ZirArrayTypeInfo *typeinfo)
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

    // We allocate memory for the string representation of this <struct ZirTypeInfo> object
    char *string_value = fl_cstring_new(0);
    
    struct ZirArrayTypeInfo *array_type = (struct ZirArrayTypeInfo*) typeinfo;

    fl_cstring_append(&string_value, "[");
    fl_cstring_vappend(&string_value, "%zu", array_type->length);
    fl_cstring_append(&string_value, "]");

    fl_cstring_append(&string_value, zir_type_to_string(array_type->member_type));

    // Update the string representation
    typeinfo->base.to_string.version = type_hash;
    typeinfo->base.to_string.value = fl_cstring_replace_realloc(typeinfo->base.to_string.value, typeinfo->base.to_string.value, string_value);

    fl_cstring_free(string_value);

    return typeinfo->base.to_string.value;
}

bool zir_type_array_equals(struct ZirArrayTypeInfo *type_a, struct ZirTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZirTypeInfo*) type_a == type_b;

    if (type_b->type != ZIR_TYPE_ARRAY)
        return false;

    struct ZirArrayTypeInfo *type_b_array = (struct ZirArrayTypeInfo*) type_b;

    return type_a->length == type_b_array->length && zir_type_equals(type_a->member_type, type_b_array->member_type);
}

bool zir_type_array_is_assignable_from(struct ZirArrayTypeInfo *target_type, struct ZirTypeInfo *from_type)
{
    if (from_type->type != ZIR_TYPE_ARRAY)
        return false;

    struct ZirArrayTypeInfo *array_from_type = (struct ZirArrayTypeInfo*) from_type;

    if (target_type->length != array_from_type->length)
        return false;

    // If rhs is an empty array, as long as the target type is not NONE, it is safe to assign
    if (target_type->length == 0 && array_from_type->member_type->type == ZIR_TYPE_NONE)
        return target_type->member_type->type != ZIR_TYPE_NONE;

    return zir_type_is_assignable_from(target_type->member_type, array_from_type->member_type);
}

bool zir_type_array_is_castable_to(struct ZirArrayTypeInfo *array_type, struct ZirTypeInfo *target_type)
{
    if (array_type == NULL || target_type == NULL)
        return false;

    // We can only cast can the array type to another compatible array type
    if (target_type->type != ZIR_TYPE_ARRAY)
        return false;

    // If target_type is the same type of the array type, we are ok to cast    
    if (zir_type_array_equals(array_type, target_type))
        return true;    

    // If we reached this point, it means that:
    //  - The target type has a different number of elements than the array type
    //  - The number of elements are equals, in that case we should check if we can cast from the array's declared/inferred
    //    type to the target declared/inferred type
    struct ZirArrayTypeInfo *target_array_type = (struct ZirArrayTypeInfo*) target_type;

    if (array_type->length != target_array_type->length)
        return false;

    return zir_type_is_castable_to(array_type->member_type, target_array_type->member_type);
}

bool zir_type_array_unify(struct ZirArrayTypeInfo *array_type, struct ZirTypeInfo *type_b, struct ZirTypeInfo **unified)
{
    if (array_type == NULL || type_b == NULL)
        return false;

    if (type_b->type == ZIR_TYPE_NONE)
    {
        if (unified) 
            *unified = (struct ZirTypeInfo*) zir_type_array_copy(array_type);
        return true;
    }

    if (type_b->type != ZIR_TYPE_ARRAY)
        return false;

    if (zir_type_array_equals(array_type, type_b))
    {
        if (unified) 
            *unified = (struct ZirTypeInfo*) zir_type_array_copy(array_type);
        return true;
    }

    struct ZirArrayTypeInfo *arr_type_b = (struct ZirArrayTypeInfo*) type_b;

    if (array_type->length != arr_type_b->length)
        return false;

    if (!zir_type_unify(array_type->member_type, arr_type_b->member_type, NULL))
        return false;
    
    if (unified)
    {
        struct ZirTypeInfo *unified_member_type = NULL;

        // If this is false, it is an unknown error...
        if (!zir_type_unify(array_type->member_type, arr_type_b->member_type, &unified_member_type))
            return false;

        struct ZirArrayTypeInfo *unified_array_type = zir_type_array_new();
        *unified = (struct ZirTypeInfo*) unified_array_type;

        unified_array_type->length = array_type->length;
        unified_array_type->member_type = unified_member_type;
        unified_array_type->source = array_type->source;

        for (size_t i=0; i < fl_array_length(array_type->members); i++)
        {
            struct ZirTypeInfo *unified_type = NULL;

            if (!zir_type_unify(unified_member_type, array_type->members[i], &unified_type))
                return false;

            zir_type_array_add_member(unified_array_type, unified_type);
        }
    }

    return true;
}

size_t zir_type_array_size(struct ZirArrayTypeInfo *typeinfo)
{
    if (!typeinfo)
        return 0;

    return typeinfo->length * zir_type_size(typeinfo->member_type);
}

void zir_type_array_free(struct ZirArrayTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->base.to_string.value != NULL)
        fl_cstring_free(typeinfo->base.to_string.value);

    zir_type_free(typeinfo->member_type);

    fl_array_free_each_pointer(typeinfo->members, (FlArrayFreeElementFunc) zir_type_free);

    fl_free(typeinfo);
}
