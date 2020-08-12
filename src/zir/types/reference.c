
#include <stdlib.h>
#include <string.h>
#include <fllib/Mem.h>
#include <fllib/Cstring.h>
#include "reference.h"

ZirReferenceType* zir_reference_type_new(ZirType *element)
{
    ZirReferenceType *type = fl_malloc(sizeof(ZirReferenceType));
    type->base.typekind = ZIR_TYPE_REFERENCE;
    type->element = element;

    return type;
}

unsigned long zir_reference_type_hash(ZirReferenceType *type)
{
    static const char *format = "[ref][e:%lu]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, zir_type_hash(type->element));

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

char* zir_reference_type_to_string(ZirReferenceType *type)
{
    if (type == NULL)
        return NULL;

    unsigned long type_hash = zir_reference_type_hash(type);

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
    char *string_value = fl_cstring_vdup("&%s", zir_type_to_string(type->element));

    // Update the string representation
    type->base.to_string.version = type_hash;
    type->base.to_string.value = fl_cstring_replace_realloc(type->base.to_string.value, type->base.to_string.value, string_value);

    fl_cstring_free(string_value);

    return type->base.to_string.value;
}

bool zir_reference_type_equals(ZirReferenceType *type_a, ZirType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (ZirType*) type_a == type_b;

    if (type_b->typekind != ZIR_TYPE_REFERENCE)
        return false;

    ZirReferenceType *type_b_ref = (ZirReferenceType*) type_b;

    return zir_type_equals(type_a->element, type_b_ref->element);
}

bool zir_reference_type_is_assignable_from(ZirReferenceType *target_type, ZirType *from_type)
{
    if (!target_type || !from_type)
        return false;

    if (from_type->typekind != ZIR_TYPE_REFERENCE)
        return false;

    ZirReferenceType *ref_from_type = (ZirReferenceType*) from_type;

    return zir_type_is_assignable_from(target_type->element, ref_from_type->element);
}

bool zir_reference_type_is_castable_to(ZirReferenceType *reference, ZirType *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // Cast between the same types are valid
    if (zir_reference_type_equals(reference, target_type))
        return true;

    if (target_type->typekind == ZIR_TYPE_REFERENCE)
        return zir_type_is_assignable_from(reference->element, ((ZirReferenceType*) target_type)->element);

    // We can cast a reference to an unsigned integer
    if (target_type->typekind == ZIR_TYPE_UINT)
        return true;

    return false;
}

size_t zir_reference_type_size(ZirReferenceType *type, size_t ref_size)
{
    if (!type)
        return 0;

    return ref_size;
}

void zir_reference_type_free(ZirReferenceType *type)
{
    if (!type)
        return;

    if (type->base.to_string.value != NULL)
        fl_cstring_free(type->base.to_string.value);

    if (type->element)
        zir_type_free(type->element);

    fl_free(type);
}
