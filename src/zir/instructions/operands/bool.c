
#include <fllib/Cstring.h>
#include "bool.h"
#include "../../types/bool.h"

ZirBoolOperand* zir_operand_bool_new(ZirBoolType *type, bool value)
{
    ZirBoolOperand *bool_operand = fl_malloc(sizeof(ZirBoolOperand));
    bool_operand->base.type = ZIR_OPERAND_BOOL;
    bool_operand->value = value;
    bool_operand->type = type;

    return bool_operand;
}

void zir_operand_bool_free(ZirBoolOperand *bool_operand)
{
    if (!bool_operand)
        return;

    if (bool_operand->type)
        zir_type_bool_free(bool_operand->type);

    fl_free(bool_operand);
}

char* zir_operand_bool_dump(ZirBoolOperand *bool_operand, char *output)
{
    fl_cstring_vappend(&output, "%s", bool_operand->value ? "true" : "false");
    return output;
}

char* zir_operand_bool_type_dump(ZirBoolOperand *bool_operand, char *output)
{
    fl_cstring_vappend(&output, "%s", zir_type_bool_to_string(bool_operand->type));
    return output;
}
