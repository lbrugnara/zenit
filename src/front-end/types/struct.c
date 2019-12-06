#include <fllib.h>
#include <stdlib.h>
#include "struct.h"

struct ZenitStructTypeInfo* zenit_type_struct_new(char *name)
{
    struct ZenitStructTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitStructTypeInfo));
    typeinfo->base.type = ZENIT_TYPE_STRUCT;
    typeinfo->name = fl_cstring_dup(name);
    // FIXME: Allocate members array
    return typeinfo;
}

void zenit_type_struct_free(struct ZenitStructTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->name)
        fl_cstring_free(typeinfo->name);

    fl_free(typeinfo);
}
