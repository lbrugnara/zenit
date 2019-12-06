#include <fllib.h>
#include "array.h"

struct ZenitArrayTypeInfo* zenit_type_array_new(void)
{
    struct ZenitArrayTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitArrayTypeInfo));
    typeinfo->base.type = ZENIT_TYPE_ARRAY;
    typeinfo->length = 0;
    typeinfo->elements = fl_array_new(sizeof(struct ZenitArrayTypeInfo*), 0);

    return typeinfo;
}

void zenit_type_array_add_element(struct ZenitArrayTypeInfo *typeinfo, struct ZenitTypeInfo *element)
{
    // Do the same for the type information
    typeinfo->elements = fl_array_append(typeinfo->elements, &element);

    size_t current_length = fl_array_length(typeinfo->elements);

    flm_assert(current_length <= typeinfo->length, "Wrong length setup in array type info");

    if (current_length < typeinfo->length)
        typeinfo->length++;
}

void zenit_type_array_free(struct ZenitArrayTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    fl_array_free_each_pointer(typeinfo->elements, (FlArrayFreeElementFunc)zenit_type_free);
    fl_free(typeinfo);
}
