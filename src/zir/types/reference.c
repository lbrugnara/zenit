#include <fllib.h>
#include <stdlib.h>
#include "reference.h"

struct ZirReferenceTypeInfo* zir_type_reference_new(struct ZirTypeInfo *element)
{
    struct ZirReferenceTypeInfo *typeinfo = fl_malloc(sizeof(struct ZirReferenceTypeInfo));
    typeinfo->base.type = ZIR_TYPE_REFERENCE;
    typeinfo->element = element;

    return typeinfo;
}

unsigned long zir_type_reference_hash(struct ZirReferenceTypeInfo *typeinfo)
{
    static const char *format = "[ref][e:%lu]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, zir_type_hash(typeinfo->element));

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

struct ZirReferenceTypeInfo* zir_type_reference_copy(struct ZirReferenceTypeInfo *src_type)
{
    if (!src_type)
        return NULL;

    return zir_type_reference_new(zir_type_copy(src_type->element));
}

char* zir_type_reference_to_string(struct ZirReferenceTypeInfo *typeinfo)
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
    char *string_value = fl_cstring_vdup("&%s", zir_type_to_string(typeinfo->element));

    // Update the string representation
    typeinfo->base.to_string.version = type_hash;
    typeinfo->base.to_string.value = fl_cstring_replace_realloc(typeinfo->base.to_string.value, typeinfo->base.to_string.value, string_value);

    fl_cstring_free(string_value);

    return typeinfo->base.to_string.value;
}

bool zir_type_reference_equals(struct ZirReferenceTypeInfo *type_a, struct ZirTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZirTypeInfo*) type_a == type_b;

    if (type_b->type != ZIR_TYPE_REFERENCE)
        return false;

    struct ZirReferenceTypeInfo *type_b_ref = (struct ZirReferenceTypeInfo*) type_b;

    return zir_type_equals(type_a->element, type_b_ref->element);
}

bool zir_type_reference_is_assignable_from(struct ZirReferenceTypeInfo *target_type, struct ZirTypeInfo *from_type)
{
    if (!target_type || !from_type)
        return false;

    if (from_type->type != ZIR_TYPE_REFERENCE)
        return false;

    struct ZirReferenceTypeInfo *ref_from_type = (struct ZirReferenceTypeInfo*) from_type;

    return zir_type_is_assignable_from(target_type->element, ref_from_type->element);
}

bool zir_type_reference_is_castable_to(struct ZirReferenceTypeInfo *reference, struct ZirTypeInfo *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // Cast between the same types are valid
    if (zir_type_reference_equals(reference, target_type))
        return true;

    // We can cast a reference to an unsigned integer
    if (target_type->type == ZIR_TYPE_UINT)
        return true;

    return false;
}

size_t zir_type_reference_size(struct ZirReferenceTypeInfo *typeinfo)
{
    if (!typeinfo)
        return 0;

    // FIXME: A reference size must be equals to the pointer size of the backend implementation
    // not sure if we can make something here
    return 0;
}

void zir_type_reference_free(struct ZirReferenceTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->base.to_string.value != NULL)
        fl_cstring_free(typeinfo->base.to_string.value);

    if (typeinfo->element)
        zir_type_free(typeinfo->element);

    fl_free(typeinfo);
}
