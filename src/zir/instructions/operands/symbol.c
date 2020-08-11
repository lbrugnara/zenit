
#include <fllib/Cstring.h>
#include "symbol.h"

ZirSymbolOperand* zir_symbol_operand_new(ZirSymbol *symbol)
{
    flm_assert(symbol != NULL, "Symbol must not be NULL");

    ZirSymbolOperand *operand = fl_malloc(sizeof(ZirSymbolOperand));
    operand->base.type = ZIR_OPERAND_SYMBOL;
    operand->symbol = symbol;

    return operand;
}

void zir_symbol_operand_free(ZirSymbolOperand *operand)
{
    if (!operand)
        return;

    fl_free(operand);
}

char* zir_symbol_operand_dump(ZirSymbolOperand *operand, char *output)
{
    fl_cstring_vappend(&output, "%s%s", operand->symbol->name && operand->symbol->name[0] == '%' ? "" : "@", operand->symbol->name);
    return output;
}

char* zir_symbol_operand_type_dump(ZirSymbolOperand *operand, char *output)
{
    fl_cstring_vappend(&output, "%s", zir_type_to_string(operand->symbol->type));
    return output;
}
