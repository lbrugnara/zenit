#include <fllib/Cstring.h>
#include "uint.h"

ZnesUintAlloc* znes_uint_alloc_new(const char *name, ZnesSegmentKind segment, size_t size, uint16_t address)
{
    ZnesUintAlloc *uint_symbol = fl_malloc(sizeof(ZnesUintAlloc));

    uint_symbol->base.address = address;
    uint_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    uint_symbol->base.segment = segment;
    uint_symbol->base.type = ZNES_ALLOC_TYPE_UINT;
    uint_symbol->base.size = size;
    
    return uint_symbol;
}

void znes_uint_alloc_free(ZnesUintAlloc *symbol)
{
    if (symbol->base.name)
        fl_cstring_free(symbol->base.name);

    fl_free(symbol);
}
