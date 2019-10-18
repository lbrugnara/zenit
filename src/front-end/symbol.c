#include "symbol.h"

CenitSymbol* cenit_symbol_new(const char *name, CenitTypeInfo *typeinfo)
{
    flm_assert(name != NULL, "Symbol name cannot be NULL");
    flm_assert(typeinfo != NULL, "Type information cannot be NULL");


    CenitSymbol *symbol = fl_malloc(sizeof(CenitSymbol));

    symbol->name = fl_cstring_dup(name);
    symbol->typeinfo.name = typeinfo->name ? fl_cstring_dup(typeinfo->name) : NULL;
    symbol->typeinfo.elements = typeinfo->elements;
    symbol->typeinfo.is_array = typeinfo->is_array;
    symbol->typeinfo.type = typeinfo->type;

    return symbol;
}

void cenit_symbol_free(CenitSymbol *symbol)
{
    if (!symbol)
        return;

    if (symbol->name)
        fl_cstring_free(symbol->name);

    if (symbol->typeinfo.name)
        fl_cstring_free(symbol->typeinfo.name);

    fl_free(symbol);
}
