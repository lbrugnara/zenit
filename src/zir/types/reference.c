#include <fllib.h>
#include <stdlib.h>
#include "reference.h"

struct ZirReferenceType* zir_type_reference_new(struct ZirType *element)
{
    struct ZirReferenceType *type = fl_malloc(sizeof(struct ZirReferenceType));
    type->base.typekind = ZIR_TYPE_REFERENCE;
    type->element = element;

    return type;
}

unsigned long zir_type_reference_hash(struct ZirReferenceType *type)
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

struct ZirReferenceType* zir_type_reference_copy(struct ZirReferenceType *src_type)
{
    if (!src_type)
        return NULL;

    return zir_type_reference_new(zir_type_copy(src_type->element));
}

char* zir_type_reference_to_string(struct ZirReferenceType *type)
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

    // We allocate memory for the string representation of this <struct ZirType> object
    char *string_value = fl_cstring_vdup("&%s", zir_type_to_string(type->element));

    // Update the string representation
    type->base.to_string.version = type_hash;
    type->base.to_string.value = fl_cstring_replace_realloc(type->base.to_string.value, type->base.to_string.value, string_value);

    fl_cstring_free(string_value);

    return type->base.to_string.value;
}

bool zir_type_reference_equals(struct ZirReferenceType *type_a, struct ZirType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZirType*) type_a == type_b;

    if (type_b->typekind != ZIR_TYPE_REFERENCE)
        return false;

    struct ZirReferenceType *type_b_ref = (struct ZirReferenceType*) type_b;

    return zir_type_equals(type_a->element, type_b_ref->element);
}

bool zir_type_reference_is_assignable_from(struct ZirReferenceType *target_type, struct ZirType *from_type)
{
    if (!target_type || !from_type)
        return false;

    if (from_type->typekind != ZIR_TYPE_REFERENCE)
        return false;

    struct ZirReferenceType *ref_from_type = (struct ZirReferenceType*) from_type;

    return zir_type_is_assignable_from(target_type->element, ref_from_type->element);
}

bool zir_type_reference_is_castable_to(struct ZirReferenceType *reference, struct ZirType *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // Cast between the same types are valid
    if (zir_type_reference_equals(reference, target_type))
        return true;

    // We can cast a reference to an unsigned integer
    if (target_type->typekind == ZIR_TYPE_UINT)
        return true;

    return false;
}

size_t zir_type_reference_size(struct ZirReferenceType *type)
{
    if (!type)
        return 0;

    // FIXME: A reference size must be equals to the pointer size of the backend implementation
    // not sure if we can make something here
    return 0;
}

void zir_type_reference_free(struct ZirReferenceType *type)
{
    if (!type)
        return;

    if (type->base.to_string.value != NULL)
        fl_cstring_free(type->base.to_string.value);

    if (type->element)
        zir_type_free(type->element);

    fl_free(type);
}
