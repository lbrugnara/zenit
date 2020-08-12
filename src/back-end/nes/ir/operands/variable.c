
#include <fllib/Cstring.h>
#include "variable.h"

ZnesVariableOperand* znes_variable_operand_new(ZnesAlloc *variable)
{
    flm_assert(variable != NULL, "Symbol must not be NULL");

    ZnesVariableOperand *operand = fl_malloc(sizeof(ZnesVariableOperand));
    operand->base.type = ZNES_OPERAND_VARIABLE;
    operand->variable = variable;

    return operand;
}

void znes_variable_operand_free(ZnesVariableOperand *operand)
{
    if (!operand)
        return;

    fl_free(operand);
}

char* znes_variable_operand_dump(ZnesVariableOperand *operand, char *output)
{
    fl_cstring_vappend(&output, "%s%s", operand->variable->name && operand->variable->name[0] == '%' ? "" : "@", operand->variable->name);
    return output;
}
