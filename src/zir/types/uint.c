#include <fllib.h>
#include <stdlib.h>
#include "uint.h"

static struct TypeMapping {
    char *string;
    enum ZirUintTypeSize size;
} type_mappings[] = {
    { "<unknown>",  ZIR_UINT_UNK  },
    { "uint8",      ZIR_UINT_8    },
    { "uint16",     ZIR_UINT_16   },
};

enum ZirUintTypeSize zir_type_uint_size_from_slice(struct FlSlice *slice)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (fl_slice_equals_sequence(slice, (FlByte*)mapping.string, strlen(mapping.string)))
            return mapping.size;
    }

    return ZIR_UINT_UNK;
}

struct ZirUintType* zir_type_uint_new(enum ZirUintTypeSize size)
{
    struct ZirUintType *type = fl_malloc(sizeof(struct ZirUintType));
    type->base.typekind = ZIR_TYPE_UINT;
    type->size = size;

    return type;
}

unsigned long zir_type_uint_hash(struct ZirUintType *type)
{
    static const char *format = "[uint][s:%s]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, zir_type_uint_to_string(type));

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

struct ZirUintType* zir_type_uint_copy(struct ZirUintType *src_type)
{
    if (!src_type)
        return NULL;

    return zir_type_uint_new(src_type->size);
}

char* zir_type_uint_to_string(struct ZirUintType *type)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (type->size == mapping.size)
            return mapping.string;
    }

    return NULL;
}

bool zir_type_uint_equals(struct ZirUintType *type_a, struct ZirType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZirType*) type_a == type_b;

    if (type_b->typekind != ZIR_TYPE_UINT)
        return false;

    return type_a->size == ((struct ZirUintType*) type_b)->size;
}

bool zir_type_uint_is_assignable_from(struct ZirUintType *target_type, struct ZirType *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    if (from_type->typekind != ZIR_TYPE_UINT)
        return false;

    struct ZirUintType *from_uint_type = (struct ZirUintType*) from_type;

    // If the "from" size is lesser or equals than the target size, it is ok to implicit cast
    return from_uint_type->size <= target_type->size;
}

bool zir_type_uint_is_castable_to(struct ZirUintType *uint_type, struct ZirType *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // It is safe to cast between uints
    return target_type->typekind == ZIR_TYPE_UINT;
}

size_t zir_type_uint_size(struct ZirUintType *type)
{
    if (!type)
        return 0;

    switch (type->size)
    {
        case ZIR_UINT_8:
            return 1;

        case ZIR_UINT_16:
            return 2;

        default: 
            break;
    }

    return 0;
}

void zir_type_uint_free(struct ZirUintType *type)
{
    if (!type)
        return;

    if (type->base.to_string.value != NULL)
        fl_cstring_free(type->base.to_string.value);

    fl_free(type);
}
