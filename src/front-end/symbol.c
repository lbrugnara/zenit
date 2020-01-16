#include "symbol.h"

struct ZenitSymbol* zenit_symbol_new(const char *name, struct ZenitTypeInfo *typeinfo)
{
    flm_assert(name != NULL, "Symbol name cannot be NULL");
    flm_assert(typeinfo != NULL, "Type information cannot be NULL");


    struct ZenitSymbol *symbol = fl_malloc(sizeof(struct ZenitSymbol));

    symbol->name = fl_cstring_dup(name);
    
    memcpy(&symbol->typeinfo, typeinfo, sizeof(symbol->typeinfo));

    return symbol;
}

void zenit_symbol_set_type(struct ZenitSymbol *symbol, struct ZenitTypeInfo *typeinfo)
{
    memcpy(&symbol->typeinfo, typeinfo, sizeof(symbol->typeinfo));
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
    fl_cstring_vappend(&output, "(symbol %s %s)", symbol->name, zenit_type_to_string(symbol->typeinfo.type));
    return output;
}
