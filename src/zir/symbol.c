#include "symbol.h"

struct ZirSymbol* zir_symbol_new(const char *name, struct ZirType *type)
{
    flm_assert(name != NULL, "Symbol name cannot be NULL");

    struct ZirSymbol *symbol = fl_malloc(sizeof(struct ZirSymbol));

    symbol->name = fl_cstring_dup(name);
    symbol->type = type;

    return symbol;
}

void zir_symbol_free(struct ZirSymbol *symbol)
{
    if (!symbol)
        return;

    if (symbol->name)
        fl_cstring_free(symbol->name);

    if (symbol->type)
        zir_type_free(symbol->type);

    fl_free(symbol);
}

char* zir_symbol_dump(struct ZirSymbol *symbol, char *output)
{
    fl_cstring_vappend(&output, "%s : %s", symbol->name, symbol->type != NULL ? zir_type_to_string(symbol->type) : "<unknown>");
    return output;
}
