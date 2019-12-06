#ifndef ZENIT_TYPE_STRUCT_H
#define ZENIT_TYPE_STRUCT_H

#include "type.h"

struct ZenitStructTypeInfo {
    struct ZenitTypeInfo base;
    char *name;
    struct ZenitTypeInfo *members;
};

struct ZenitStructTypeInfo* zenit_type_struct_new(char *name);
void zenit_type_struct_free(struct ZenitStructTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_STRUCT_H */
