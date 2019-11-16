#include "operand.h"

void zenit_ir_operand_free(struct ZenitIrOperand *operand)
{
    // We don't care about ZENIT_IR_OPERAND_SYMBOL because the
    // symbol table is in charge of that
    if (operand->type == ZENIT_IR_OPERAND_VALUE)
    {
        zenit_ir_value_free(operand->operand.value);
    }
}

char* zenit_ir_operand_dump(struct ZenitIrOperand *operand, char *output)
{
    if (operand->type == ZENIT_IR_OPERAND_VALUE)
        return zenit_ir_value_dump(operand->operand.value, output);

    if (operand->type == ZENIT_IR_OPERAND_SYMBOL)
    {
        fl_cstring_vappend(&output, "@%s", operand->operand.symbol->name);
        return output;
    }

    if (operand->type == ZENIT_IR_OPERAND_REF)
    {
        fl_cstring_vappend(&output, "ref @%s", operand->operand.symbol->name);
        return output;
    }

    return output;
}
