#include <fllib/Cstring.h>
#include "temp.h"

struct ZenitNesTempSymbol* zenit_nes_symbol_temp_new(const char *name, struct ZirType *zir_type)
{
    // We need to get the symbol size to make sure it fits
    size_t symbol_size = zir_type->typekind == ZIR_TYPE_REFERENCE ? 2 /*bytes*/ : zir_type_size(zir_type);

    struct ZenitNesTempSymbol *temp_symbol = fl_malloc(sizeof(struct ZenitNesTempSymbol));

    temp_symbol->base.symkind = ZENIT_NES_SYMBOL_TEMP;
    temp_symbol->base.name = fl_cstring_dup(name);
    temp_symbol->base.address = 0; // Mind that it being a temp symbol means we don't actually use the address
    temp_symbol->base.segment = ZENIT_NES_SEGMENT_TEMP;
    temp_symbol->base.size = symbol_size;

    return temp_symbol;
}

void zenit_nes_symbol_temp_free(struct ZenitNesTempSymbol *symbol)
{
    if (symbol->base.name)
        fl_cstring_free(symbol->base.name);

    fl_free(symbol);
}
