
#include <stdlib.h>
#include <fllib/Cstring.h>
#include "reference.h"

ZenitReferenceType* zenit_reference_type_new(ZenitType *element)
{
    ZenitReferenceType *type = fl_malloc(sizeof(ZenitReferenceType));
    type->base.typekind = ZENIT_TYPE_REFERENCE;
    type->element = element;

    return type;
}

unsigned long zenit_reference_type_hash(ZenitReferenceType *type)
{
    static const char *format = "[ref][e:%lu]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, zenit_type_hash(type->element));

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

char* zenit_reference_type_to_string(ZenitReferenceType *type)
{
    if (type == NULL)
        return NULL;

    unsigned long type_hash = zenit_reference_type_hash(type);

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
    char *string_value = fl_cstring_vdup("&%s", zenit_type_to_string(type->element));

    // Update the string representation
    type->base.to_string.version = type_hash;
    type->base.to_string.value = fl_cstring_replace_realloc(type->base.to_string.value, type->base.to_string.value, string_value);

    fl_cstring_free(string_value);

    return type->base.to_string.value;
}

bool zenit_reference_type_equals(ZenitReferenceType *type_a, ZenitType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (ZenitType*) type_a == type_b;

    if (type_b->typekind != ZENIT_TYPE_REFERENCE)
        return false;

    ZenitReferenceType *type_b_ref = (ZenitReferenceType*) type_b;

    return zenit_type_equals(type_a->element, type_b_ref->element);
}

bool zenit_reference_type_is_assignable_from(ZenitReferenceType *target_type, ZenitType *from_type)
{
    if (!target_type || !from_type)
        return false;

    if (from_type->typekind != ZENIT_TYPE_REFERENCE)
        return false;

    ZenitReferenceType *ref_from_type = (ZenitReferenceType*) from_type;

    return zenit_type_is_assignable_from(target_type->element, ref_from_type->element);
}

bool zenit_reference_type_is_castable_to(ZenitReferenceType *reference, ZenitType *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // Cast between the same types are valid
    if (zenit_reference_type_equals(reference, target_type))
        return true;

    if (target_type->typekind == ZENIT_TYPE_REFERENCE)
        return zenit_type_is_assignable_from(reference->element, ((ZenitReferenceType*) target_type)->element);

    // We can cast a reference to an unsigned integer
    if (target_type->typekind == ZENIT_TYPE_UINT)
        return true;

    return false;
}

bool zenit_reference_type_can_unify(ZenitReferenceType *ref_type, ZenitType *type_b)
{
    if (ref_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE)
        return true;

    if (type_b->typekind != ZENIT_TYPE_REFERENCE)
        return false;

    if (zenit_reference_type_equals(ref_type, type_b))
        return true;

    ZenitReferenceType *ref_type_b = (ZenitReferenceType*) type_b;

    return zenit_type_can_unify(ref_type->element, ref_type_b->element);
}

void zenit_reference_type_free(ZenitReferenceType *type)
{
    if (!type)
        return;

    if (type->base.to_string.value != NULL)
        fl_cstring_free(type->base.to_string.value);

    fl_free(type);
}
