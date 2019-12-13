#include "operand.h"
#include "array.h"
#include "uint.h"
#include "reference.h"
#include "symbol.h"

void zir_operand_free(struct ZirOperand *operand)
{
    if (!operand)
        return;

    switch (operand->type)
    {
        case ZIR_OPERAND_UINT:
            zir_operand_uint_free((struct ZirUintOperand*)operand);
            break;

        case ZIR_OPERAND_ARRAY:
            zir_operand_array_free((struct ZirArrayOperand*)operand);
            break;

        case ZIR_OPERAND_SYMBOL:
            zir_operand_symbol_free((struct ZirSymbolOperand*)operand);
            break;

        case ZIR_OPERAND_REFERENCE:
            zir_operand_reference_free((struct ZirReferenceOperand*)operand);
            break;
    }
}

char* zir_operand_dump(struct ZirOperand *operand, char *output)
{
    switch (operand->type)
    {
        case ZIR_OPERAND_UINT:
            return zir_operand_uint_dump((struct ZirUintOperand*)operand, output);

        case ZIR_OPERAND_ARRAY:
            return zir_operand_array_dump((struct ZirArrayOperand*)operand, output);

        case ZIR_OPERAND_SYMBOL:
            return zir_operand_symbol_dump((struct ZirSymbolOperand*)operand, output);

        case ZIR_OPERAND_REFERENCE:
            return zir_operand_reference_dump((struct ZirReferenceOperand*)operand, output);
    }

    return output;
}

char* zir_operand_type_dump(struct ZirOperand *operand, char *output)
{
    switch (operand->type)
    {
        case ZIR_OPERAND_UINT:
            return zir_operand_uint_type_dump((struct ZirUintOperand*)operand, output);

        case ZIR_OPERAND_ARRAY:
            return zir_operand_array_type_dump((struct ZirArrayOperand*)operand, output);

        case ZIR_OPERAND_SYMBOL:
            return zir_operand_symbol_type_dump((struct ZirSymbolOperand*)operand, output);

        case ZIR_OPERAND_REFERENCE:
            return zir_operand_reference_type_dump((struct ZirReferenceOperand*)operand, output);
    }

    return output;
}
