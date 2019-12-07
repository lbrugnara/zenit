#include <fllib.h>
#include "array.h"

struct ZenitArrayTypeInfo* zenit_type_array_new(void)
{
    struct ZenitArrayTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitArrayTypeInfo));
    typeinfo->base.type = ZENIT_TYPE_ARRAY;
    typeinfo->members = fl_array_new(sizeof(struct ZenitArrayTypeInfo*), 0);

    return typeinfo;
}

void zenit_type_array_add_member(struct ZenitArrayTypeInfo *typeinfo, struct ZenitTypeInfo *element)
{
    // Do the same for the type information
    typeinfo->members = fl_array_append(typeinfo->members, &element);
}

void zenit_type_array_free(struct ZenitArrayTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    fl_array_free_each_pointer(typeinfo->members, (FlArrayFreeElementFunc) zenit_type_free);

    fl_free(typeinfo);
}
