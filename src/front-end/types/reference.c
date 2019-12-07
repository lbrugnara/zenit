#include <fllib.h>
#include <stdlib.h>
#include "reference.h"

struct ZenitReferenceTypeInfo* zenit_type_reference_new(struct ZenitTypeInfo *element)
{
    struct ZenitReferenceTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitReferenceTypeInfo));
    typeinfo->base.type = ZENIT_TYPE_REFERENCE;
    typeinfo->element = element;

    return typeinfo;
}

void zenit_type_reference_free(struct ZenitReferenceTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->element)
        zenit_type_free(typeinfo->element);

    fl_free(typeinfo);
}
