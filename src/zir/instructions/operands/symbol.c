
#include <fllib/Cstring.h>
#include "symbol.h"

ZirSymbolOperand* zir_operand_symbol_new(ZirSymbol *symbol)
{
    flm_assert(symbol != NULL, "Symbol must not be NULL");

    ZirSymbolOperand *operand = fl_malloc(sizeof(ZirSymbolOperand));
    operand->base.type = ZIR_OPERAND_SYMBOL;
    operand->symbol = symbol;

    return operand;
}

void zir_operand_symbol_free(ZirSymbolOperand *operand)
{
    if (!operand)
        return;

    fl_free(operand);
}

char* zir_operand_symbol_dump(ZirSymbolOperand *operand, char *output)
{
    fl_cstring_vappend(&output, "%s%s", operand->symbol->name && operand->symbol->name[0] == '%' ? "" : "@", operand->symbol->name);
    return output;
}

char* zir_operand_symbol_type_dump(ZirSymbolOperand *operand, char *output)
{
    fl_cstring_vappend(&output, "%s", zir_type_to_string(operand->symbol->type));
    return output;
}
