#include <fllib/Cstring.h>
#include "symbol.h"

ZirSymbol* zir_symbol_new(const char *name, ZirType *type)
{
    flm_assert(name != NULL, "Symbol name cannot be NULL");

    ZirSymbol *symbol = fl_malloc(sizeof(ZirSymbol));

    symbol->name = fl_cstring_dup(name);
    symbol->type = type;

    return symbol;
}

void zir_symbol_free(ZirSymbol *symbol)
{
    if (!symbol)
        return;

    if (symbol->name)
        fl_cstring_free(symbol->name);

    if (symbol->type)
        zir_type_free(symbol->type);

    fl_free(symbol);
}

char* zir_symbol_dump(ZirSymbol *symbol, char *output)
{
    fl_cstring_vappend(&output, "%s: %s", symbol->name, symbol->type != NULL ? zir_type_to_string(symbol->type) : "<unknown>");
    return output;
}
