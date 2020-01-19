#include "symbol.h"

struct ZenitSymbol* zenit_symbol_new(const char *name, struct ZenitType *type)
{
    flm_assert(name != NULL, "Symbol name cannot be NULL");
    flm_assert(type != NULL, "Type information cannot be NULL");


    struct ZenitSymbol *symbol = fl_malloc(sizeof(struct ZenitSymbol));

    symbol->name = fl_cstring_dup(name);
    symbol->type = type;

    return symbol;
}

void zenit_symbol_free(struct ZenitSymbol *symbol)
{
    if (!symbol)
        return;

    if (symbol->name)
        fl_cstring_free(symbol->name);

    fl_free(symbol);
}

char* zenit_symbol_dump(struct ZenitSymbol *symbol, char *output)
{
    fl_cstring_vappend(&output, "(symbol %s %s)", symbol->name, zenit_type_to_string(symbol->type));
    return output;
}
