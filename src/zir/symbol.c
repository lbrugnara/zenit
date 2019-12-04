#include "symbol.h"

struct ZenitIrSymbol* zenit_ir_symbol_new(const char *name, struct ZenitIrTypeInfo *typeinfo, bool temporal)
{
    flm_assert(name != NULL, "Symbol name cannot be NULL");

    struct ZenitIrSymbol *symbol = fl_malloc(sizeof(struct ZenitIrSymbol));

    symbol->name = fl_cstring_dup(name);
    symbol->temporal = temporal;

    if (typeinfo)
        zenit_ir_type_copy(&symbol->typeinfo, typeinfo);

    return symbol;
}

void zenit_ir_symbol_free(struct ZenitIrSymbol *symbol)
{
    if (!symbol)
        return;

    if (symbol->name)
        fl_cstring_free(symbol->name);

    if (symbol->typeinfo.name)
        fl_cstring_free(symbol->typeinfo.name);

    fl_free(symbol);
}
