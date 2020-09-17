#include <fllib/Cstring.h>
#include <fllib/Array.h>
#include "array.h"
#include "../operands/array.h"
#include "../utils.h"

ZnesArrayAlloc* znes_array_alloc_new(const char *name, ZnesSegmentKind segment, size_t size, uint16_t address)
{
    ZnesArrayAlloc *array_symbol = fl_malloc(sizeof(ZnesArrayAlloc));
    array_symbol->base.address = address;
    array_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    array_symbol->base.segment = segment;
    array_symbol->base.type = ZNES_ALLOC_TYPE_ARRAY;
    array_symbol->base.size = size;
    array_symbol->elements = fl_array_new(sizeof(ZnesAlloc*), 0);

    return array_symbol;
}

ZnesAlloc* znes_array_alloc_add_element(ZnesArrayAlloc *array_symbol, ZnesAlloc *element)
{
    array_symbol->elements = fl_array_append(array_symbol->elements, &element);
    return element;
}

void znes_array_alloc_free(ZnesArrayAlloc *array_symbol)
{
    if (array_symbol->base.name)
        fl_cstring_free(array_symbol->base.name);

    fl_array_free_each_pointer(array_symbol->elements, (FlArrayFreeElementFunc) znes_alloc_free);

    fl_free(array_symbol);
}
