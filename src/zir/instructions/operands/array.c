#include <fllib.h>
#include "array.h"
#include "../../types/array.h"

struct ZirArrayOperand* zir_operand_array_new(struct ZirArrayTypeInfo *typeinfo)
{
    struct ZirArrayOperand *operand = fl_malloc(sizeof(struct ZirArrayOperand));
    operand->base.type = ZIR_OPERAND_ARRAY;
    operand->elements = fl_array_new(sizeof(struct ZirOperand*), 0);
    operand->typeinfo = typeinfo;

    return operand;
}

void zir_operand_array_free(struct ZirArrayOperand *operand)
{
    if (!operand)
        return;

    fl_array_free(operand->elements);

    if (operand->typeinfo)
        zir_type_array_free(operand->typeinfo);

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

char* zir_operand_array_type_dump(struct ZirArrayOperand *array, char *output)
{
    fl_cstring_vappend(&output, "%s", zir_type_array_to_string(array->typeinfo));
    return output;
}

