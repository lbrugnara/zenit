#include <fllib.h>
#include <stdlib.h>
#include "uint.h"

static struct TypeMapping {
    char *string;
    enum ZenitUintTypeSize size;
} type_mappings[] = {
    { "<unknown>",  ZENIT_UINT_UNK  },
    { "uint8",      ZENIT_UINT_8    },
    { "uint16",     ZENIT_UINT_16   },
};

struct ZenitUintTypeInfo* zenit_type_uint_new(enum ZenitTypeSource source, enum ZenitUintTypeSize size)
{
    struct ZenitUintTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitUintTypeInfo));
    typeinfo->base.type = ZENIT_TYPE_UINT;
    typeinfo->base.source = source;
    typeinfo->size = size;

    return typeinfo;
}

enum ZenitUintTypeSize zenit_type_uint_size_from_slice(struct FlSlice *slice)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (fl_slice_equals_sequence(slice, (FlByte*)mapping.string, strlen(mapping.string)))
            return mapping.size;
    }

    return ZENIT_UINT_UNK;
}

unsigned long zenit_type_uint_hash(struct ZenitUintTypeInfo *typeinfo)
{
    static const char *format = "[uint][s:%s]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, zenit_type_uint_to_string(typeinfo));

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

struct ZenitUintTypeInfo* zenit_type_uint_copy(struct ZenitUintTypeInfo *src_type)
{
    if (!src_type)
        return NULL;

    return zenit_type_uint_new(src_type->base.source, src_type->size);
}

char* zenit_type_uint_to_string(struct ZenitUintTypeInfo *typeinfo)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (typeinfo->size == mapping.size)
            return mapping.string;
    }

    return NULL;
}

bool zenit_type_uint_equals(struct ZenitUintTypeInfo *type_a, struct ZenitTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZenitTypeInfo*) type_a == type_b;

    if (type_b->type != ZENIT_TYPE_UINT)
        return false;

    return type_a->size == ((struct ZenitUintTypeInfo*) type_b)->size;
}

bool zenit_type_uint_is_assignable_from(struct ZenitUintTypeInfo *target_type, struct ZenitTypeInfo *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    if (from_type->type != ZENIT_TYPE_UINT)
        return false;

    struct ZenitUintTypeInfo *from_uint_type = (struct ZenitUintTypeInfo*) from_type;

    // If the "from" size is lesser or equals than the target size, it is ok to implicit cast
    return from_uint_type->size <= target_type->size;
}

bool zenit_type_uint_is_castable_to(struct ZenitUintTypeInfo *uint_type, struct ZenitTypeInfo *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // It is safe to cast between uints
    return target_type->type == ZENIT_TYPE_UINT;
}

struct ZenitTypeInfo* zenit_type_uint_unify(struct ZenitUintTypeInfo *uint_type, struct ZenitTypeInfo *type_b)
{
    if (uint_type == NULL || type_b == NULL)
        return NULL;

    if (type_b->type != ZENIT_TYPE_NONE && type_b->type != ZENIT_TYPE_UINT)
        return NULL;

    if (type_b->type == ZENIT_TYPE_NONE || zenit_type_uint_equals(uint_type, type_b))
    {
        struct ZenitTypeInfo *unified = (struct ZenitTypeInfo*) zenit_type_uint_copy(uint_type);
        unified->source = ZENIT_TYPE_SRC_INFERRED;
        return unified;
    }

    // At this point, type_b must be a uint
    struct ZenitUintTypeInfo *uint_b = (struct ZenitUintTypeInfo*) type_b;
    struct ZenitTypeInfo *unified = (struct ZenitTypeInfo*) zenit_type_uint_copy(uint_type->size > uint_b->size ? uint_type : uint_b);
    unified->source = ZENIT_TYPE_SRC_INFERRED;
    return unified;
}

bool zenit_type_uint_can_unify(struct ZenitUintTypeInfo *uint_type, struct ZenitTypeInfo *type_b)
{
    if (uint_type == NULL || type_b == NULL)
        return false;

    if (type_b->type != ZENIT_TYPE_NONE && type_b->type != ZENIT_TYPE_UINT)
        return false;

    return true;
}

void zenit_type_uint_free(struct ZenitUintTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->base.to_string.value != NULL)
        fl_cstring_free(typeinfo->base.to_string.value);

    fl_free(typeinfo);
}
