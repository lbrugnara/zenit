#include "operand.h"

void zir_operand_free(struct ZirOperand *operand)
{
    // We don't care about ZIR_OPERAND_SYMBOL because the
    // symbol table is in charge of that
    if (operand->type == ZIR_OPERAND_VALUE)
    {
        zir_value_free(operand->operand.value);
    }
}

char* zir_operand_dump(struct ZirOperand *operand, char *output)
{
    if (operand->type == ZIR_OPERAND_VALUE)
        return zir_value_dump(operand->operand.value, output);

    if (operand->type == ZIR_OPERAND_SYMBOL)
    {
        fl_cstring_vappend(&output, "%s%s", operand->operand.symbol->temporal ? "%" : "@", operand->operand.symbol->name);
        return output;
    }

    if (operand->type == ZIR_OPERAND_REFERENCE)
    {
        fl_cstring_vappend(&output, "ref %s%s", operand->operand.symbol->temporal ? "%" : "@", operand->operand.symbol->name);
        return output;
    }

    return output;
}
