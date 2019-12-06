#ifndef ZENIT_TYPE_ARRAY_H
#define ZENIT_TYPE_ARRAY_H

#include <stdlib.h>
#include "type.h"

struct ZenitArrayTypeInfo {
    struct ZenitTypeInfo base;
    struct ZenitTypeInfo **members;
    size_t length;
};

struct ZenitArrayTypeInfo* zenit_type_array_new(void);
void zenit_type_array_free(struct ZenitArrayTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_ARRAY_H */
