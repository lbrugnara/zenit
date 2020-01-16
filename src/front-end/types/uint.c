#include <fllib.h>
#include <stdlib.h>
#include "uint.h"
#include "typeinfo.h"

static struct TypeMapping {
    char *string;
    enum ZenitUintTypeSize size;
} type_mappings[] = {
    { "<unknown>",  ZENIT_UINT_UNK  },
    { "uint8",      ZENIT_UINT_8    },
    { "uint16",     ZENIT_UINT_16   },
};

struct ZenitUintType* zenit_type_uint_new(enum ZenitUintTypeSize size)
{
    struct ZenitUintType *typeinfo = fl_malloc(sizeof(struct ZenitUintType));
    typeinfo->base.typekind = ZENIT_TYPE_UINT;
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

unsigned long zenit_type_uint_hash(struct ZenitUintType *typeinfo)
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

struct ZenitUintType* zenit_type_uint_copy(struct ZenitUintType *src_type)
{
    if (!src_type)
        return NULL;

    return zenit_type_uint_new(src_type->size);
}

char* zenit_type_uint_to_string(struct ZenitUintType *typeinfo)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (typeinfo->size == mapping.size)
            return mapping.string;
    }

    return NULL;
}

bool zenit_type_uint_equals(struct ZenitUintType *type_a, struct ZenitType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZenitType*) type_a == type_b;

    if (type_b->typekind != ZENIT_TYPE_UINT)
        return false;

    return type_a->size == ((struct ZenitUintType*) type_b)->size;
}

bool zenit_type_uint_is_assignable_from(struct ZenitUintType *target_type, struct ZenitType *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    if (from_type->typekind != ZENIT_TYPE_UINT)
        return false;

    struct ZenitUintType *from_uint_type = (struct ZenitUintType*) from_type;

    // If the "from" size is lesser or equals than the target size, it is ok to implicit cast
    return from_uint_type->size <= target_type->size;
}

bool zenit_type_uint_is_castable_to(struct ZenitUintType *uint_type, struct ZenitType *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // It is safe to cast between uints
    return target_type->typekind == ZENIT_TYPE_UINT;
}

struct ZenitTypeInfo* zenit_type_uint_unify(struct ZenitUintType *uint_type, struct ZenitType *type_b)
{
    if (uint_type == NULL || type_b == NULL)
        return NULL;

    if (type_b->typekind != ZENIT_TYPE_NONE && type_b->typekind != ZENIT_TYPE_UINT)
        return NULL;

    if (type_b->typekind == ZENIT_TYPE_NONE || zenit_type_uint_equals(uint_type, type_b))
    {
        struct ZenitTypeInfo *unified = zenit_typeinfo_new_uint(ZENIT_TYPE_SRC_INFERRED, false, zenit_type_uint_copy(uint_type));
        return unified;
    }

    // At this point, type_b must be a uint
    struct ZenitUintType *uint_b = (struct ZenitUintType*) type_b;
    struct ZenitTypeInfo *unified = zenit_typeinfo_new_uint(ZENIT_TYPE_SRC_INFERRED, false, zenit_type_uint_copy(uint_type->size > uint_b->size ? uint_type : uint_b));
    return unified;
}

bool zenit_type_uint_can_unify(struct ZenitUintType *uint_type, struct ZenitType *type_b)
{
    if (uint_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind != ZENIT_TYPE_NONE && type_b->typekind != ZENIT_TYPE_UINT)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE)
        return true;

    return true;
    /*struct ZenitUintType *uint_type_b = (struct ZenitUintType*) type_b;

    if (uint_type->size == uint_type_b->size)
        return true;*/

    /*// If B is smaller than A, the only way to "unify" them is by promoting B to
    // the size of A, which means the type of B must have been inferred or must be intrinsic
    // to the object holding the type, because a "user hinted" type cannot be changed.
    if (uint_type->size > uint_type_b->size)
        return uint_type_b->base.source != ZENIT_TYPE_SRC_HINT;

    // This is the same as above but in the case A is smaller than B
    return uint_type->base.source != ZENIT_TYPE_SRC_HINT;*/
}

void zenit_type_uint_free(struct ZenitUintType *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->base.to_string.value != NULL)
        fl_cstring_free(typeinfo->base.to_string.value);

    fl_free(typeinfo);
}
