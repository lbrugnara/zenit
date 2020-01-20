#ifndef ZIR_TYPE_UINT_H
#define ZIR_TYPE_UINT_H

#include <fllib.h>
#include "type.h"

enum ZirUintTypeSize {
    ZIR_UINT_UNK,
    ZIR_UINT_8,
    ZIR_UINT_16
};

struct ZirUintType {
    struct ZirType base;
    enum ZirUintTypeSize size;
};

struct ZirUintType* zir_type_uint_new(enum ZirUintTypeSize size);
enum ZirUintTypeSize zir_type_uint_size_from_slice(struct FlSlice *slice);
unsigned long zir_type_uint_hash(struct ZirUintType *type);
struct ZirUintType* zir_type_uint_copy(struct ZirUintType *src_type);
char* zir_type_uint_to_string(struct ZirUintType *type);
bool zir_type_uint_equals(struct ZirUintType *type_a, struct ZirType *type_b);
bool zir_type_uint_is_assignable_from(struct ZirUintType *target_type, struct ZirType *value_type);
bool zir_type_uint_is_castable_to(struct ZirUintType *primitive_type, struct ZirType *target_type);
size_t zir_type_uint_size(struct ZirUintType *type);
void zir_type_uint_free(struct ZirUintType *type);

#endif /* ZIR_TYPE_UINT_H */
