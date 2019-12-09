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

    if (typeinfo->base.to_string.value != NULL)
        fl_cstring_free(typeinfo->base.to_string.value);

    fl_free(typeinfo);
}
