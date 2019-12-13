#ifndef ZENIT_TYPE_UINT_H
#define ZENIT_TYPE_UINT_H

#include <fllib.h>
#include "type.h"

enum ZenitUintTypeSize {
    ZENIT_UINT_UNK,
    ZENIT_UINT_8,
    ZENIT_UINT_16
};

struct ZenitUintTypeInfo {
    struct ZenitTypeInfo base;
    enum ZenitUintTypeSize size;
};

struct ZenitUintTypeInfo* zenit_type_uint_new(enum ZenitTypeSource source, enum ZenitUintTypeSize size);
enum ZenitUintTypeSize zenit_type_uint_size_from_slice(struct FlSlice *slice);
unsigned long zenit_type_uint_hash(struct ZenitUintTypeInfo *typeinfo);
struct ZenitUintTypeInfo* zenit_type_uint_copy(struct ZenitUintTypeInfo *src_type);
char* zenit_type_uint_to_string(struct ZenitUintTypeInfo *typeinfo);
bool zenit_type_uint_equals(struct ZenitUintTypeInfo *type_a, struct ZenitTypeInfo *type_b);
bool zenit_type_uint_is_assignable_from(struct ZenitUintTypeInfo *target_type, struct ZenitTypeInfo *value_type);
bool zenit_type_uint_is_castable_to(struct ZenitUintTypeInfo *primitive_type, struct ZenitTypeInfo *target_type);
bool zenit_type_uint_unify(struct ZenitUintTypeInfo *primitive_type, struct ZenitTypeInfo *type_b, struct ZenitTypeInfo **unified);
void zenit_type_uint_free(struct ZenitUintTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_UINT_H */
