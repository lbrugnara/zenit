#include <fllib.h>
#include <stdlib.h>
#include "none.h"

struct ZenitTypeInfo* zenit_type_none_new(void)
{
    struct ZenitTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitTypeInfo));
    typeinfo->type = ZENIT_TYPE_NONE;

    return typeinfo;
}
