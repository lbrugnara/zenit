
#include <stdlib.h>
#include <fllib/Cstring.h>
#include "uint.h"

static struct TypeMapping {
    char *string;
    ZirUintTypeSize size;
} type_mappings[] = {
    { "<unknown>",  ZIR_UINT_UNK  },
    { "uint8",      ZIR_UINT_8    },
    { "uint16",     ZIR_UINT_16   },
};

ZirUintType* zir_type_uint_new(ZirUintTypeSize size)
{
    ZirUintType *type = fl_malloc(sizeof(ZirUintType));
    type->base.typekind = ZIR_TYPE_UINT;
    type->size = size;

    return type;
}

unsigned long zir_type_uint_hash(ZirUintType *type)
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

char* zir_type_uint_to_string(ZirUintType *type)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (type->size == mapping.size)
            return mapping.string;
    }

    return NULL;
}

bool zir_type_uint_equals(ZirUintType *type_a, ZirType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (ZirType*) type_a == type_b;

    if (type_b->typekind != ZIR_TYPE_UINT)
        return false;

    return type_a->size == ((ZirUintType*) type_b)->size;
}

bool zir_type_uint_is_assignable_from(ZirUintType *target_type, ZirType *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    if (from_type->typekind != ZIR_TYPE_UINT)
        return false;

    ZirUintType *from_uint_type = (ZirUintType*) from_type;

    // If the "from" size is lesser or equals than the target size, it is ok to implicit cast
    return from_uint_type->size <= target_type->size;
}

bool zir_type_uint_is_castable_to(ZirUintType *uint_type, ZirType *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // It is safe to cast between uints
    return target_type->typekind == ZIR_TYPE_UINT;
}

size_t zir_type_uint_size(ZirUintType *type)
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

void zir_type_uint_free(ZirUintType *type)
{
    if (!type)
        return;

    if (type->base.to_string.value != NULL)
        fl_cstring_free(type->base.to_string.value);

    fl_free(type);
}
