#ifndef ZENIT_TYPE_ARRAY_H
#define ZENIT_TYPE_ARRAY_H

#include <stdlib.h>
#include "type.h"

enum ZenitArrayTypeInfoSource {
    ZENIT_ARRAY_TYPE_DECL,
    ZENIT_ARRAY_TYPE_INFER,
};

struct ZenitArrayTypeInfo {
    struct ZenitTypeInfo base;
    struct ZenitTypeInfo **members;
    enum ZenitArrayTypeInfoSource source;
    struct ZenitTypeInfo *member_type;
    size_t length;
};

struct ZenitArrayTypeInfo* zenit_type_array_new(void);
void zenit_type_array_add_member(struct ZenitArrayTypeInfo *typeinfo, struct ZenitTypeInfo *element);
void zenit_type_array_free(struct ZenitArrayTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_ARRAY_H */
