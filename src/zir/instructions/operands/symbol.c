
#include <fllib/Cstring.h>
#include "symbol.h"

struct ZirSymbolOperand* zir_operand_symbol_new(struct ZirSymbol *symbol)
{
    flm_assert(symbol != NULL, "Symbol must not be NULL");

    struct ZirSymbolOperand *operand = fl_malloc(sizeof(struct ZirSymbolOperand));
    operand->base.type = ZIR_OPERAND_SYMBOL;
    operand->symbol = symbol;

    return operand;
}

void zir_operand_symbol_free(struct ZirSymbolOperand *operand)
{
    if (!operand)
        return;

    fl_free(operand);
}

char* zir_operand_symbol_dump(struct ZirSymbolOperand *operand, char *output)
{
    fl_cstring_vappend(&output, "%s%s", operand->symbol->name && operand->symbol->name[0] == '%' ? "" : "@", operand->symbol->name);
    return output;
}

char* zir_operand_symbol_type_dump(struct ZirSymbolOperand *operand, char *output)
{
    fl_cstring_vappend(&output, "%s", zir_type_to_string(operand->symbol->type));
    return output;
}
