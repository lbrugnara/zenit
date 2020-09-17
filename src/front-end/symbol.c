#include <fllib/Cstring.h>
#include "symbol.h"

ZenitSymbol* zenit_symbol_new(const char *name, char *mangled_name, ZenitType *type)
{
    flm_assert(name != NULL, "Symbol name cannot be NULL");
    flm_assert(type != NULL, "Type information cannot be NULL");


    ZenitSymbol *symbol = fl_malloc(sizeof(ZenitSymbol));

    symbol->name = fl_cstring_dup(name);
    symbol->mangled_name = mangled_name;
    symbol->type = type;

    return symbol;
}

void zenit_symbol_free(ZenitSymbol *symbol)
{
    if (!symbol)
        return;

    if (symbol->name) fl_cstring_free(symbol->name);
    if (symbol->mangled_name) fl_cstring_free(symbol->mangled_name);

    fl_free(symbol);
}

char* zenit_symbol_dump(ZenitSymbol *symbol, char *output)
{
    fl_cstring_vappend(&output, "(symbol %s %s)", symbol->name, zenit_type_to_string(symbol->type));
    return output;
}
