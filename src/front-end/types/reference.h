#ifndef ZENIT_TYPE_REFERENCE_H
#define ZENIT_TYPE_REFERENCE_H

#include "type.h"

struct ZenitReferenceTypeInfo {
    struct ZenitTypeInfo base;
    struct ZenitTypeInfo *element;
};

struct ZenitReferenceTypeInfo* zenit_type_reference_new(struct ZenitTypeInfo *element);
void zenit_type_reference_free(struct ZenitReferenceTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_REFERENCE_H */
