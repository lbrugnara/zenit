#include "operand.h"
#include "array.h"
#include "primitive.h"
#include "reference.h"
#include "symbol.h"

void zir_operand_free(struct ZirOperand *operand)
{
    if (!operand)
        return;

    switch (operand->type)
    {
        case ZIR_OPERAND_PRIMITIVE:
            zir_operand_primitive_free((struct ZirPrimitiveOperand*)operand);
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
        case ZIR_OPERAND_PRIMITIVE:
            return zir_operand_primitive_dump((struct ZirPrimitiveOperand*)operand, output);

        case ZIR_OPERAND_ARRAY:
            return zir_operand_array_dump((struct ZirArrayOperand*)operand, output);

        case ZIR_OPERAND_SYMBOL:
            return zir_operand_symbol_dump((struct ZirSymbolOperand*)operand, output);

        case ZIR_OPERAND_REFERENCE:
            return zir_operand_reference_dump((struct ZirReferenceOperand*)operand, output);
    }

    return output;
}
