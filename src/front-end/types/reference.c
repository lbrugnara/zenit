#include <fllib.h>
#include <stdlib.h>
#include "reference.h"

struct ZenitReferenceTypeInfo* zenit_type_reference_new(struct ZenitTypeInfo *element)
{
    struct ZenitReferenceTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitReferenceTypeInfo));
    typeinfo->base.type = ZENIT_TYPE_REFERENCE;
    typeinfo->element = element;

    return typeinfo;
}

unsigned long zenit_type_reference_hash(struct ZenitReferenceTypeInfo *typeinfo)
{
    static const char *format = "[ref][e:%lu]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, zenit_type_hash(typeinfo->element));

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

struct ZenitReferenceTypeInfo* zenit_type_reference_copy(struct ZenitReferenceTypeInfo *src_type)
{
    if (!src_type)
        return NULL;

    return zenit_type_reference_new(zenit_type_copy(src_type->element));
}

char* zenit_type_reference_to_string(struct ZenitReferenceTypeInfo *typeinfo)
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
    char *string_value = fl_cstring_vdup("&%s", zenit_type_to_string(typeinfo->element));

    // Update the string representation
    typeinfo->base.to_string.version = type_hash;
    typeinfo->base.to_string.value = fl_cstring_replace_realloc(typeinfo->base.to_string.value, typeinfo->base.to_string.value, string_value);

    fl_cstring_free(string_value);

    return typeinfo->base.to_string.value;
}

bool zenit_type_reference_equals(struct ZenitReferenceTypeInfo *type_a, struct ZenitTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZenitTypeInfo*) type_a == type_b;

    if (type_b->type != ZENIT_TYPE_REFERENCE)
        return false;

    struct ZenitReferenceTypeInfo *type_b_ref = (struct ZenitReferenceTypeInfo*) type_b;

    return zenit_type_equals(type_a->element, type_b_ref->element);
}

bool zenit_type_reference_is_assignable_from(struct ZenitReferenceTypeInfo *target_type, struct ZenitTypeInfo *from_type)
{
    if (!target_type || !from_type)
        return false;

    if (from_type->type != ZENIT_TYPE_REFERENCE)
        return false;

    struct ZenitReferenceTypeInfo *ref_from_type = (struct ZenitReferenceTypeInfo*) from_type;

    return zenit_type_is_assignable_from(target_type->element, ref_from_type->element);
}

bool zenit_type_reference_is_castable_to(struct ZenitReferenceTypeInfo *reference, struct ZenitTypeInfo *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // Cast between the same types are valid
    if (zenit_type_reference_equals(reference, target_type))
        return true;

    // We can cast a reference to an unsigned integer
    if (target_type->type == ZENIT_TYPE_UINT)
        return true;

    return false;
}

bool zenit_type_reference_unify(struct ZenitReferenceTypeInfo *ref_type, struct ZenitTypeInfo *type_b, struct ZenitTypeInfo **unified)
{
    if (ref_type == NULL || type_b == NULL)
        return false;

    if (type_b->type == ZENIT_TYPE_NONE)
    {
        if (unified)
            *unified = (struct ZenitTypeInfo*) zenit_type_reference_copy(ref_type);
        return true;
    }

    if (type_b->type != ZENIT_TYPE_REFERENCE)
        return false;

    if (zenit_type_reference_equals(ref_type, type_b))
    {
        if (unified)
            *unified = (struct ZenitTypeInfo*) zenit_type_reference_copy(ref_type);
        return true;
    }

    struct ZenitReferenceTypeInfo *ref_type_b = (struct ZenitReferenceTypeInfo*) type_b;

    if (!zenit_type_unify(ref_type->element, ref_type_b->element, NULL))
        return false;

    if (unified)
    {
        struct ZenitTypeInfo *unified_element_type = NULL;
    
        if (!zenit_type_unify(ref_type->element, ref_type_b->element, &unified_element_type))
            return false;
    
        *unified = (struct ZenitTypeInfo*) zenit_type_reference_new(unified_element_type);
    }

    return true;
}

void zenit_type_reference_free(struct ZenitReferenceTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->base.to_string.value != NULL)
        fl_cstring_free(typeinfo->base.to_string.value);

    fl_free(typeinfo);
}
