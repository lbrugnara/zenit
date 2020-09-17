#include <fllib/Cstring.h>
#include "bool.h"

ZnesBoolAlloc* znes_bool_alloc_new(const char *name, ZnesSegmentKind segment, size_t size, uint16_t address)
{
    ZnesBoolAlloc *bool_symbol = fl_malloc(sizeof(ZnesBoolAlloc));
    bool_symbol->base.address = address;
    bool_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    bool_symbol->base.segment = segment;
    bool_symbol->base.type = ZNES_ALLOC_TYPE_BOOL;
    bool_symbol->base.size = size;
    
    return bool_symbol;
}

void znes_bool_alloc_free(ZnesBoolAlloc *symbol)
{
    if (symbol->base.name)
        fl_cstring_free(symbol->base.name);

    fl_free(symbol);
}
