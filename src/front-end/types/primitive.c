#include <fllib.h>
#include <stdlib.h>
#include "primitive.h"

struct ZenitPrimitiveTypeInfo* zenit_type_primitive_new(enum ZenitType type)
{
    if (!zenit_type_is_primitive(type))
        return NULL;
        
    struct ZenitPrimitiveTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitPrimitiveTypeInfo));
    typeinfo->base.type = type;

    return typeinfo;
}

void zenit_type_primitive_free(struct ZenitPrimitiveTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    fl_free(typeinfo);
}
