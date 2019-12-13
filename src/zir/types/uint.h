#ifndef ZIR_TYPE_UINT_H
#define ZIR_TYPE_UINT_H

#include <fllib.h>
#include "type.h"

enum ZirUintTypeSize {
    ZIR_UINT_UNK,
    ZIR_UINT_8,
    ZIR_UINT_16
};

struct ZirUintTypeInfo {
    struct ZirTypeInfo base;
    enum ZirUintTypeSize size;
};

struct ZirUintTypeInfo* zir_type_uint_new(enum ZirUintTypeSize size);
enum ZirUintTypeSize zir_type_uint_size_from_slice(struct FlSlice *slice);
unsigned long zir_type_uint_hash(struct ZirUintTypeInfo *typeinfo);
struct ZirUintTypeInfo* zir_type_uint_copy(struct ZirUintTypeInfo *src_type);
char* zir_type_uint_to_string(struct ZirUintTypeInfo *typeinfo);
bool zir_type_uint_equals(struct ZirUintTypeInfo *type_a, struct ZirTypeInfo *type_b);
bool zir_type_uint_is_assignable_from(struct ZirUintTypeInfo *target_type, struct ZirTypeInfo *value_type);
bool zir_type_uint_is_castable_to(struct ZirUintTypeInfo *primitive_type, struct ZirTypeInfo *target_type);
bool zir_type_uint_unify(struct ZirUintTypeInfo *primitive_type, struct ZirTypeInfo *type_b, struct ZirTypeInfo **unified);
size_t zir_type_uint_size(struct ZirUintTypeInfo *typeinfo);
void zir_type_uint_free(struct ZirUintTypeInfo *typeinfo);

#endif /* ZIR_TYPE_UINT_H */
