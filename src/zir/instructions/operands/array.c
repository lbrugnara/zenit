#include <fllib.h>
#include "array.h"
#include "../../type.h"

struct ZirArrayOperand* zir_operand_array_new(void)
{
    struct ZirArrayOperand *operand = fl_malloc(sizeof(struct ZirArrayOperand));
    operand->base.type = ZIR_OPERAND_ARRAY;
    operand->elements = fl_array_new(sizeof(struct ZirOperand*), 0);

    return operand;
}

void zir_operand_array_free(struct ZirArrayOperand *operand)
{
    if (!operand)
        return;

    zir_type_free(&operand->base.typeinfo);

    fl_array_free(operand->elements);

    fl_free(operand);
}

char* zir_operand_array_dump(struct ZirArrayOperand *array, char *output)
{
    fl_cstring_append(&output, "[ ");
    if (array->elements)
    {
        for (size_t i=0; i < fl_array_length(array->elements); i++)
        {
            if (i > 0)
                fl_cstring_append(&output, ", ");

            struct ZirOperand *operand = array->elements[i];
            output = zir_operand_dump(operand, output);
        }
    }
    fl_cstring_append(&output, " ]");

    return output;
}
