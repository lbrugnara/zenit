#ifndef ZENIT_TYPE_STRUCT_H
#define ZENIT_TYPE_STRUCT_H

#include "type.h"

struct ZenitStructTypeInfo {
    struct ZenitTypeInfo base;
    const char *name;
    struct ZenitTypeInfo *members;
};

#endif /* ZENIT_TYPE_STRUCT_H */
