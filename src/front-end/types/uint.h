#ifndef ZENIT_TYPE_UINT_H
#define ZENIT_TYPE_UINT_H

#include <fllib.h>
#include "type.h"

enum ZenitUintTypeSize {
    ZENIT_UINT_UNK,
    ZENIT_UINT_8,
    ZENIT_UINT_16
};

struct ZenitUintType {
    struct ZenitType base;
    enum ZenitUintTypeSize size;
};

struct ZenitUintType* zenit_type_uint_new(enum ZenitUintTypeSize size);
enum ZenitUintTypeSize zenit_type_uint_size_from_slice(struct FlSlice *slice);
unsigned long zenit_type_uint_hash(struct ZenitUintType *typeinfo);
struct ZenitUintType* zenit_type_uint_copy(struct ZenitUintType *src_type);
char* zenit_type_uint_to_string(struct ZenitUintType *typeinfo);
bool zenit_type_uint_equals(struct ZenitUintType *type_a, struct ZenitType *type_b);
bool zenit_type_uint_is_assignable_from(struct ZenitUintType *target_type, struct ZenitType *value_type);
bool zenit_type_uint_is_castable_to(struct ZenitUintType *primitive_type, struct ZenitType *target_type);
struct ZenitTypeInfo* zenit_type_uint_unify(struct ZenitUintType *primitive_type, struct ZenitType *type_b);
bool zenit_type_uint_can_unify(struct ZenitUintType *primitive_type, struct ZenitType *type_b);
void zenit_type_uint_free(struct ZenitUintType *typeinfo);

#endif /* ZENIT_TYPE_UINT_H */
