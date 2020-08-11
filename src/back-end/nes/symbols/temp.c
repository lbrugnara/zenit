#include <fllib/Cstring.h>
#include "temp.h"

ZnesTempSymbol* znes_temp_symbol_new(const char *name, ZirType *zir_type)
{
    // We need to get the symbol size to make sure it fits
    size_t symbol_size = zir_type->typekind == ZIR_TYPE_REFERENCE ? 2 /*bytes*/ : zir_type_size(zir_type);

    ZnesTempSymbol *temp_symbol = fl_malloc(sizeof(ZnesTempSymbol));

    temp_symbol->base.symkind = ZNES_SYMBOL_TEMP;
    temp_symbol->base.name = fl_cstring_dup(name);
    temp_symbol->base.address = 0; // Mind that it being a temp symbol means we don't actually use the address
    temp_symbol->base.segment = ZNES_SEGMENT_TEMP;
    temp_symbol->base.size = symbol_size;

    return temp_symbol;
}

void znes_temp_symbol_free(ZnesTempSymbol *symbol)
{
    if (symbol->base.name)
        fl_cstring_free(symbol->base.name);

    fl_free(symbol);
}
