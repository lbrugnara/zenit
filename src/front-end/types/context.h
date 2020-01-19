#ifndef ZENIT_TYPE_SYSTEM_H
#define ZENIT_TYPE_SYSTEM_H

#include <fllib.h>

#include "type.h"
#include "array.h"
#include "none.h"
#include "uint.h"
#include "reference.h"
#include "struct.h"

struct ZenitTypeContext {
    FlHashtable pool;
    FlList uniques;
};

struct ZenitTypeContext* zenit_type_ctx_new(void);

struct ZenitArrayType* zenit_type_ctx_new_array(struct ZenitTypeContext *type_ctx, struct ZenitType *member_type);
struct ZenitType* zenit_type_ctx_new_none(struct ZenitTypeContext *type_ctx);
struct ZenitReferenceType* zenit_type_ctx_new_reference(struct ZenitTypeContext *type_ctx, struct ZenitType *element);
struct ZenitStructType* zenit_type_ctx_new_struct(struct ZenitTypeContext *type_ctx, char *name);
struct ZenitUintType* zenit_type_ctx_new_uint(struct ZenitTypeContext *type_ctx, enum ZenitUintTypeSize size);

struct ZenitType* zenit_type_ctx_copy_type(struct ZenitTypeContext *type_ctx, struct ZenitType *type);
struct ZenitArrayType* zenit_type_ctx_copy_array(struct ZenitTypeContext *type_ctx, struct ZenitArrayType *src_array_type);
struct ZenitReferenceType* zenit_type_ctx_copy_reference(struct ZenitTypeContext *type_ctx, struct ZenitReferenceType *src_ref_type);
struct ZenitStructType* zenit_type_ctx_copy_struct(struct ZenitTypeContext *type_ctx, struct ZenitStructType *src_struct_type);
struct ZenitUintType* zenit_type_ctx_copy_uint(struct ZenitTypeContext *type_ctx, struct ZenitUintType *src_uint_type);

bool zenit_type_ctx_unify_types(struct ZenitTypeContext *type_ctx, struct ZenitType *type_a, struct ZenitType *type_b, struct ZenitType **dest);

void zenit_type_ctx_free(struct ZenitTypeContext *system);

#endif /* ZENIT_TYPE_SYSTEM_H */
