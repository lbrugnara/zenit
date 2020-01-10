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

struct ZirUintTypeInfo* zir_type_uint_new(enum ZirUintTypeSize size)
{
    struct ZirUintTypeInfo *typeinfo = fl_malloc(sizeof(struct ZirUintTypeInfo));
    typeinfo->base.type = ZIR_TYPE_UINT;
    typeinfo->size = size;

    return typeinfo;
}

unsigned long zir_type_uint_hash(struct ZirUintTypeInfo *typeinfo)
{
    static const char *format = "[uint][s:%s]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, zir_type_uint_to_string(typeinfo));

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

struct ZirUintTypeInfo* zir_type_uint_copy(struct ZirUintTypeInfo *src_type)
{
    if (!src_type)
        return NULL;

    return zir_type_uint_new(src_type->size);
}

char* zir_type_uint_to_string(struct ZirUintTypeInfo *typeinfo)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (typeinfo->size == mapping.size)
            return mapping.string;
    }

    return NULL;
}

bool zir_type_uint_equals(struct ZirUintTypeInfo *type_a, struct ZirTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZirTypeInfo*) type_a == type_b;

    if (type_b->type != ZIR_TYPE_UINT)
        return false;

    return type_a->size == ((struct ZirUintTypeInfo*) type_b)->size;
}

bool zir_type_uint_is_assignable_from(struct ZirUintTypeInfo *target_type, struct ZirTypeInfo *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    if (from_type->type != ZIR_TYPE_UINT)
        return false;

    struct ZirUintTypeInfo *from_uint_type = (struct ZirUintTypeInfo*) from_type;

    // If the "from" size is lesser or equals than the target size, it is ok to implicit cast
    return from_uint_type->size <= target_type->size;
}

bool zir_type_uint_is_castable_to(struct ZirUintTypeInfo *uint_type, struct ZirTypeInfo *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // It is safe to cast between uints
    return target_type->type == ZIR_TYPE_UINT;
}

size_t zir_type_uint_size(struct ZirUintTypeInfo *typeinfo)
{
    if (!typeinfo)
        return 0;

    switch (typeinfo->size)
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

void zir_type_uint_free(struct ZirUintTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->base.to_string.value != NULL)
        fl_cstring_free(typeinfo->base.to_string.value);

    fl_free(typeinfo);
}
