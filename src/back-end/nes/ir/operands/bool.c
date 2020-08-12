
#include <fllib/Cstring.h>
#include "bool.h"

ZnesBoolOperand* znes_bool_operand_new(bool value)
{
    ZnesBoolOperand *bool_operand = fl_malloc(sizeof(ZnesBoolOperand));
    bool_operand->base.type = ZNES_OPERAND_BOOL;
    bool_operand->value = value;

    return bool_operand;
}

void znes_bool_operand_free(ZnesBoolOperand *bool_operand)
{
    if (!bool_operand)
        return;

    fl_free(bool_operand);
}

char* znes_bool_operand_dump(ZnesBoolOperand *bool_operand, char *output)
{
    fl_cstring_vappend(&output, "%s", bool_operand->value ? "true" : "false");
    return output;
}
