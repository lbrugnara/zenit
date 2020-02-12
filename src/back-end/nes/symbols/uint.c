#include <fllib/Cstring.h>
#include "uint.h"

struct ZenitNesUintSymbol* zenit_nes_symbol_uint_new(const char *name, struct ZirUintType *zir_uint_type, enum ZenitNesSegment segment, uint16_t address)
{
    struct ZenitNesUintSymbol *uint_symbol = fl_malloc(sizeof(struct ZenitNesUintSymbol));

    uint_symbol->base.address = address;
    uint_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    uint_symbol->base.segment = segment;
    uint_symbol->base.symkind = ZENIT_NES_SYMBOL_UINT;
    uint_symbol->base.size = zir_type_uint_size(zir_uint_type);
    
    return uint_symbol;
}

void zenit_nes_symbol_uint_free(struct ZenitNesUintSymbol *symbol)
{
    if (symbol->base.name)
        fl_cstring_free(symbol->base.name);

    fl_free(symbol);
}
