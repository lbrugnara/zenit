#ifndef ZENIT_TYPE_PRIMITIVE_H
#define ZENIT_TYPE_PRIMITIVE_H

#include "type.h"

struct ZenitPrimitiveTypeInfo {
    struct ZenitTypeInfo base;
};

struct ZenitPrimitiveTypeInfo* zenit_type_primitive_new(enum ZenitType type);
void zenit_type_primitive_free(struct ZenitPrimitiveTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_PRIMITIVE_H */
