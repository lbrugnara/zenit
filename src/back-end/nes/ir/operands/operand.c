#include "operand.h"
#include "array.h"
#include "bool.h"
#include "uint.h"
#include "reference.h"
#include "struct.h"
#include "variable.h"

void znes_operand_free(ZnesOperand *operand)
{
    if (!operand)
        return;

    switch (operand->type)
    {
        case ZNES_OPERAND_UINT:
            znes_uint_operand_free((ZnesUintOperand*) operand);
            break;

        case ZNES_OPERAND_BOOL:
            znes_bool_operand_free((ZnesBoolOperand*) operand);
            break;

        case ZNES_OPERAND_ARRAY:
            znes_array_operand_free((ZnesArrayOperand*) operand);
            break;

        case ZNES_OPERAND_STRUCT:
            znes_struct_operand_free((ZnesStructOperand*) operand);
            break;

        case ZNES_OPERAND_VARIABLE:
            znes_variable_operand_free((ZnesVariableOperand*) operand);
            break;

        case ZNES_OPERAND_REFERENCE:
            znes_reference_operand_free((ZnesReferenceOperand*) operand);
            break;
    }
}

char* znes_operand_dump(ZnesOperand *operand, char *output)
{
    switch (operand->type)
    {
        case ZNES_OPERAND_UINT:
            return znes_uint_operand_dump((ZnesUintOperand*) operand, output);

        case ZNES_OPERAND_BOOL:
            return znes_bool_operand_dump((ZnesBoolOperand*) operand, output);

        case ZNES_OPERAND_ARRAY:
            return znes_array_operand_dump((ZnesArrayOperand*) operand, output);

        case ZNES_OPERAND_STRUCT:
            return znes_struct_operand_dump((ZnesStructOperand*) operand, output);

        case ZNES_OPERAND_VARIABLE:
            return znes_variable_operand_dump((ZnesVariableOperand*) operand, output);

        case ZNES_OPERAND_REFERENCE:
            return znes_reference_operand_dump((ZnesReferenceOperand*) operand, output);
    }

    return output;
}

size_t znes_operand_size(ZnesOperand *operand)
{
    switch (operand->type)
    {
        case ZNES_OPERAND_UINT:
            return znes_uint_operand_size((ZnesUintOperand*) operand);

        case ZNES_OPERAND_BOOL:
            return znes_bool_operand_size((ZnesBoolOperand*) operand);

        case ZNES_OPERAND_ARRAY:
            return znes_array_operand_size((ZnesArrayOperand*) operand);

        case ZNES_OPERAND_STRUCT:
            return znes_struct_operand_size((ZnesStructOperand*) operand);

        case ZNES_OPERAND_VARIABLE:
            return znes_variable_operand_size((ZnesVariableOperand*) operand);

        case ZNES_OPERAND_REFERENCE:
            return znes_reference_operand_size((ZnesReferenceOperand*) operand);
    }

    return 0;
}
