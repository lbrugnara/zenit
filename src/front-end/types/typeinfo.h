#ifndef ZENIT_TYPEINFO_H
#define ZENIT_TYPEINFO_H

#include <stdbool.h>
#include "type.h"
#include "system.h"

static struct ZenitTypeInfo* zenit_typeinfo_new(enum ZenitTypeSource source, bool sealed, struct ZenitType *type)
{
    struct ZenitTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitTypeInfo));
    typeinfo->type = type;
    typeinfo->source = source;
    typeinfo->sealed = sealed;

    return typeinfo;
}

static inline struct ZenitTypeInfo* zenit_typeinfo_new_uint(enum ZenitTypeSource source, bool sealed, struct ZenitUintType *uint_type)
{
    return zenit_typeinfo_new(source, sealed, (struct ZenitType*) uint_type);
}

static inline struct ZenitTypeInfo* zenit_typeinfo_new_array(enum ZenitTypeSource source, bool sealed, struct ZenitArrayType *array_type)
{
    return zenit_typeinfo_new(source, sealed, (struct ZenitType*) array_type);
}

static inline struct ZenitTypeInfo* zenit_typeinfo_new_reference(enum ZenitTypeSource source, bool sealed, struct ZenitReferenceType *ref_type)
{
    return zenit_typeinfo_new(source, sealed, (struct ZenitType*) ref_type);
}

static inline struct ZenitTypeInfo* zenit_typeinfo_new_struct(enum ZenitTypeSource source, bool sealed, struct ZenitStructType *struct_type)
{
    return zenit_typeinfo_new(source, sealed, (struct ZenitType*) struct_type);
}

static inline struct ZenitTypeInfo* zenit_typeinfo_copy(struct ZenitTypeInfo *typeinfo)
{
    return zenit_typeinfo_new(typeinfo->source, typeinfo->sealed, zenit_type_copy(typeinfo->type));
}

static inline void zenit_typeinfo_free(struct ZenitTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->type)
        zenit_type_free(typeinfo->type);

    fl_free(typeinfo);
}

#endif
