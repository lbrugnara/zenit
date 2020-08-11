#include <fllib/Cstring.h>
#include "bool.h"
#include "../../../zir/types/bool.h"

ZnesBoolSymbol* znes_bool_symbol_new(const char *name, ZirBoolType *zir_bool_type, ZnesSegment segment, uint16_t address)
{
    ZnesBoolSymbol *bool_symbol = fl_malloc(sizeof(ZnesBoolSymbol));
    bool_symbol->base.address = address;
    bool_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    bool_symbol->base.segment = segment;
    bool_symbol->base.symkind = ZNES_SYMBOL_BOOL;
    bool_symbol->base.size = zir_bool_type_size(zir_bool_type);
    
    return bool_symbol;
}

void znes_bool_symbol_free(ZnesBoolSymbol *symbol)
{
    if (symbol->base.name)
        fl_cstring_free(symbol->base.name);

    fl_free(symbol);
}
