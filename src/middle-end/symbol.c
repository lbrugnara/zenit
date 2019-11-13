#include "symbol.h"

struct ZirSymbol* zir_symbol_new(const char *name, struct ZirTypeInfo *typeinfo)
{
    flm_assert(name != NULL, "Symbol name cannot be NULL");
    flm_assert(typeinfo != NULL, "Type information cannot be NULL");


    struct ZirSymbol *symbol = fl_malloc(sizeof(struct ZirSymbol));

    symbol->name = fl_cstring_dup(name);
    zir_type_copy(&symbol->typeinfo, typeinfo);

    return symbol;
}

void zir_symbol_free(struct ZirSymbol *symbol)
{
    if (!symbol)
        return;

    if (symbol->name)
        fl_cstring_free(symbol->name);

    if (symbol->typeinfo.name)
        fl_cstring_free(symbol->typeinfo.name);

    fl_free(symbol);
}
