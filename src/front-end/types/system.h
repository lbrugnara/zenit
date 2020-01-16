#ifndef ZENIT_TYPE_SYSTEM_H
#define ZENIT_TYPE_SYSTEM_H

#include <fllib.h>

#include "type.h"
#include "array.h"
#include "none.h"
#include "uint.h"
#include "reference.h"
#include "struct.h"

struct ZenitTypeSystem {
    FlHashtable pool;
    FlList uniques;
};

struct ZenitTypeSystem* zenit_typesys_new(void);

struct ZenitArrayType* zenit_typesys_new_array(struct ZenitTypeSystem *typesys, struct ZenitType *member_type);
struct ZenitType* zenit_typesys_new_none(struct ZenitTypeSystem *typesys);
struct ZenitReferenceType* zenit_typesys_new_reference(struct ZenitTypeSystem *typesys, struct ZenitType *element);
struct ZenitStructType* zenit_typesys_new_struct(struct ZenitTypeSystem *typesys, char *name);
struct ZenitUintType* zenit_typesys_new_uint(struct ZenitTypeSystem *typesys, enum ZenitUintTypeSize size);

struct ZenitType* zenit_typesys_copy_type(struct ZenitTypeSystem *typesys, struct ZenitType *type);
struct ZenitArrayType* zenit_typesys_copy_array(struct ZenitTypeSystem *typesys, struct ZenitArrayType *src_array_type);
struct ZenitReferenceType* zenit_typesys_copy_reference(struct ZenitTypeSystem *typesys, struct ZenitReferenceType *src_ref_type);
struct ZenitStructType* zenit_typesys_copy_struct(struct ZenitTypeSystem *typesys, struct ZenitStructType *src_struct_type);
struct ZenitUintType* zenit_typesys_copy_uint(struct ZenitTypeSystem *typesys, struct ZenitUintType *src_uint_type);

bool zenit_typesys_unify_types(struct ZenitTypeSystem *typesys, struct ZenitType *type_a, struct ZenitType *type_b, struct ZenitTypeInfo *dest);

void zenit_typesys_free(struct ZenitTypeSystem *system);

#endif /* ZENIT_TYPE_SYSTEM_H */
