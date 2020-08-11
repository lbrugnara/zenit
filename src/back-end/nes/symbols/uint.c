#include <fllib/Cstring.h>
#include "uint.h"

ZnesUintSymbol* znes_uint_symbol_new(const char *name, ZirUintType *zir_uint_type, ZnesSegment segment, uint16_t address)
{
    ZnesUintSymbol *uint_symbol = fl_malloc(sizeof(ZnesUintSymbol));

    uint_symbol->base.address = address;
    uint_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    uint_symbol->base.segment = segment;
    uint_symbol->base.symkind = ZNES_SYMBOL_UINT;
    uint_symbol->base.size = zir_uint_type_size(zir_uint_type);
    
    return uint_symbol;
}

void znes_uint_symbol_free(ZnesUintSymbol *symbol)
{
    if (symbol->base.name)
        fl_cstring_free(symbol->base.name);

    fl_free(symbol);
}
