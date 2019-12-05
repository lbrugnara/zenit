#include <fllib.h>
#include "symbol.h"

struct ZirSymbolOperand* zir_operand_symbol_new(struct ZirSymbol *symbol)
{
    flm_assert(symbol != NULL, "Symbol must not be NULL");

    struct ZirSymbolOperand *operand = fl_malloc(sizeof(struct ZirSymbolOperand));
    operand->base.type = ZIR_OPERAND_SYMBOL;
    operand->symbol = symbol;

    zir_type_copy(&operand->base.typeinfo, &symbol->typeinfo);

    return operand;
}

void zir_operand_symbol_free(struct ZirSymbolOperand *operand)
{
    if (!operand)
        return;

    zir_type_free(&operand->base.typeinfo);

    fl_free(operand);
}

char* zir_operand_symbol_dump(struct ZirSymbolOperand *operand, char *output)
{
    fl_cstring_vappend(&output, "%s%s", operand->symbol->temporal ? "%" : "@", operand->symbol->name);
    return output;
}
