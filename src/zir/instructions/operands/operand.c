#include "operand.h"
#include "array.h"
#include "bool.h"
#include "uint.h"
#include "reference.h"
#include "struct.h"
#include "symbol.h"

void zir_operand_free(ZirOperand *operand)
{
    if (!operand)
        return;

    switch (operand->type)
    {
        case ZIR_OPERAND_UINT:
            zir_uint_operand_free((ZirUintOperand*) operand);
            break;

        case ZIR_OPERAND_BOOL:
            zir_bool_operand_free((ZirBoolOperand*) operand);
            break;

        case ZIR_OPERAND_ARRAY:
            zir_array_operand_free((ZirArrayOperand*) operand);
            break;

        case ZIR_OPERAND_STRUCT:
            zir_struct_operand_free((ZirStructOperand*) operand);
            break;

        case ZIR_OPERAND_SYMBOL:
            zir_symbol_operand_free((ZirSymbolOperand*) operand);
            break;

        case ZIR_OPERAND_REFERENCE:
            zir_reference_operand_free((ZirReferenceOperand*) operand);
            break;
    }
}

char* zir_operand_dump(ZirOperand *operand, char *output)
{
    switch (operand->type)
    {
        case ZIR_OPERAND_UINT:
            return zir_uint_operand_dump((ZirUintOperand*) operand, output);

        case ZIR_OPERAND_BOOL:
            return zir_bool_operand_dump((ZirBoolOperand*) operand, output);

        case ZIR_OPERAND_ARRAY:
            return zir_array_operand_dump((ZirArrayOperand*) operand, output);

        case ZIR_OPERAND_STRUCT:
            return zir_struct_operand_dump((ZirStructOperand*) operand, output);

        case ZIR_OPERAND_SYMBOL:
            return zir_symbol_operand_dump((ZirSymbolOperand*) operand, output);

        case ZIR_OPERAND_REFERENCE:
            return zir_reference_operand_dump((ZirReferenceOperand*) operand, output);
    }

    return output;
}

char* zir_operand_type_dump(ZirOperand *operand, char *output)
{
    switch (operand->type)
    {
        case ZIR_OPERAND_UINT:
            return zir_uint_operand_type_dump((ZirUintOperand*) operand, output);

        case ZIR_OPERAND_BOOL:
            return zir_bool_operand_type_dump((ZirBoolOperand*) operand, output);

        case ZIR_OPERAND_ARRAY:
            return zir_array_operand_type_dump((ZirArrayOperand*) operand, output);

        case ZIR_OPERAND_STRUCT:
            return zir_struct_operand_type_dump((ZirStructOperand*) operand, output);

        case ZIR_OPERAND_SYMBOL:
            return zir_symbol_operand_type_dump((ZirSymbolOperand*) operand, output);

        case ZIR_OPERAND_REFERENCE:
            return zir_reference_operand_type_dump((ZirReferenceOperand*) operand, output);
    }

    return output;
}
