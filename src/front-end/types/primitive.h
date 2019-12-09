#ifndef ZENIT_TYPE_PRIMITIVE_H
#define ZENIT_TYPE_PRIMITIVE_H

#include <fllib.h>
#include "type.h"

struct ZenitPrimitiveTypeInfo {
    struct ZenitTypeInfo base;
};

struct ZenitPrimitiveTypeInfo* zenit_type_primitive_new(enum ZenitType type);
enum ZenitType zenit_type_primitive_from_slice(struct FlSlice *slice);
unsigned long zenit_type_primitive_hash(struct ZenitPrimitiveTypeInfo *typeinfo);
struct ZenitPrimitiveTypeInfo* zenit_type_primitive_copy(struct ZenitPrimitiveTypeInfo *src_type);
char* zenit_type_primitive_to_string(struct ZenitPrimitiveTypeInfo *typeinfo);
bool zenit_type_primitive_equals(struct ZenitPrimitiveTypeInfo *type_a, struct ZenitTypeInfo *type_b);
bool zenit_type_primitive_is_assignable_from(struct ZenitPrimitiveTypeInfo *target_type, struct ZenitTypeInfo *value_type);
void zenit_type_primitive_free(struct ZenitPrimitiveTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_PRIMITIVE_H */
