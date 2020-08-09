
#include <stdlib.h>
#include <fllib/Cstring.h>
#include "uint.h"

static struct TypeMapping {
    char *string;
    ZenitUintTypeSize size;
} type_mappings[] = {
    { "<unknown>",  ZENIT_UINT_UNK  },
    { "uint8",      ZENIT_UINT_8    },
    { "uint16",     ZENIT_UINT_16   },
};

ZenitUintType* zenit_type_uint_new(ZenitUintTypeSize size)
{
    ZenitUintType *type = fl_malloc(sizeof(ZenitUintType));
    type->base.typekind = ZENIT_TYPE_UINT;
    type->size = size;

    return type;
}

ZenitUintTypeSize zenit_type_uint_size_from_slice(struct FlSlice *slice)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (fl_slice_equals_sequence(slice, (FlByte*)mapping.string, strlen(mapping.string)))
            return mapping.size;
    }

    return ZENIT_UINT_UNK;
}

unsigned long zenit_type_uint_hash(ZenitUintType *type)
{
    static const char *format = "[uint][s:%s]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, zenit_type_uint_to_string(type));

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

char* zenit_type_uint_to_string(ZenitUintType *type)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (type->size == mapping.size)
            return mapping.string;
    }

    return NULL;
}

bool zenit_type_uint_equals(ZenitUintType *type_a, ZenitType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (ZenitType*) type_a == type_b;

    if (type_b->typekind != ZENIT_TYPE_UINT)
        return false;

    return type_a->size == ((ZenitUintType*) type_b)->size;
}

bool zenit_type_uint_is_assignable_from(ZenitUintType *target_type, ZenitType *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    if (from_type->typekind != ZENIT_TYPE_UINT)
        return false;

    ZenitUintType *from_uint_type = (ZenitUintType*) from_type;

    // If the "from" size is lesser or equals than the target size, it is ok to implicit cast
    return from_uint_type->size <= target_type->size;
}

bool zenit_type_uint_is_castable_to(ZenitUintType *uint_type, ZenitType *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // It is safe to cast between uints
    return target_type->typekind == ZENIT_TYPE_UINT;
}

bool zenit_type_uint_can_unify(ZenitUintType *uint_type, ZenitType *type_b)
{
    if (uint_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind != ZENIT_TYPE_NONE && type_b->typekind != ZENIT_TYPE_UINT)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE)
        return true;

    return true;
}

void zenit_type_uint_free(ZenitUintType *type)
{
    if (!type)
        return;

    if (type->base.to_string.value != NULL)
        fl_cstring_free(type->base.to_string.value);

    fl_free(type);
}
