#include <fllib/Cstring.h>
#include "temp.h"

ZnesTempAlloc* znes_temp_alloc_new(const char *name, size_t size)
{
    ZnesTempAlloc *temp_symbol = fl_malloc(sizeof(ZnesTempAlloc));

    temp_symbol->base.type = ZNES_ALLOC_TYPE_TEMP;
    temp_symbol->base.name = fl_cstring_dup(name);
    temp_symbol->base.address = 0; // Mind that it being a temp symbol means we don't actually use the address
    temp_symbol->base.segment = ZNES_SEGMENT_TEMP;
    temp_symbol->base.size = size;

    return temp_symbol;
}

void znes_temp_alloc_free(ZnesTempAlloc *symbol)
{
    if (symbol->base.name)
        fl_cstring_free(symbol->base.name);

    fl_free(symbol);
}
