#ifndef ZENIT_TYPE_ARRAY_H
#define ZENIT_TYPE_ARRAY_H

#include <stdlib.h>
#include "type.h"

struct ZenitArrayTypeInfo {
    struct ZenitTypeInfo base;
    struct ZenitTypeInfo **members;
};

struct ZenitArrayTypeInfo* zenit_type_array_new(void);
void zenit_type_array_add_member(struct ZenitArrayTypeInfo *typeinfo, struct ZenitTypeInfo *element);
void zenit_type_array_free(struct ZenitArrayTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_ARRAY_H */
