#include "symbol.h"

struct ZenitSymbol* zenit_symbol_new(const char *name, struct ZenitTypeInfo *typeinfo)
{
    flm_assert(name != NULL, "Symbol name cannot be NULL");
    flm_assert(typeinfo != NULL, "Type information cannot be NULL");


    struct ZenitSymbol *symbol = fl_malloc(sizeof(struct ZenitSymbol));

    symbol->name = fl_cstring_dup(name);
    symbol->typeinfo = typeinfo;

    return symbol;
}

void zenit_symbol_set_type(struct ZenitSymbol *symbol, struct ZenitTypeInfo *typeinfo)
{
    zenit_type_free(symbol->typeinfo);
    symbol->typeinfo = zenit_type_copy(typeinfo);
}

void zenit_symbol_free(struct ZenitSymbol *symbol)
{
    if (!symbol)
        return;

    if (symbol->name)
        fl_cstring_free(symbol->name);

    if (symbol->typeinfo)
        zenit_type_free(symbol->typeinfo);

    fl_free(symbol);
}

char* zenit_symbol_dump(struct ZenitSymbol *symbol, char *output)
{
    fl_cstring_vappend(&output, "%s: %s\n", symbol->name, symbol->typeinfo != NULL ? zenit_type_to_string(symbol->typeinfo) : "<no type info>");
    return output;
}
