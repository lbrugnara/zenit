#include <fllib.h>
#include "array.h"
#include "../../types/array.h"

struct ZirArrayOperand* zir_operand_array_new(struct ZirArrayType *type)
{
    struct ZirArrayOperand *operand = fl_malloc(sizeof(struct ZirArrayOperand));
    operand->base.type = ZIR_OPERAND_ARRAY;
    operand->elements = fl_array_new(sizeof(struct ZirOperand*), 0);
    operand->type = type;

    return operand;
}

void zir_operand_array_free(struct ZirArrayOperand *operand)
{
    if (!operand)
        return;

    for (size_t i=0; i < fl_array_length(operand->elements); i++)
    {
        if (operand->elements[i]->owner == NULL)
            zir_operand_free(operand->elements[i]);
    }

    fl_array_free(operand->elements);

    if (operand->type)
        zir_type_array_free(operand->type);

    fl_free(operand);
}

char* zir_operand_array_dump(struct ZirArrayOperand *array, char *output)
{
    fl_cstring_append(&output, "[ ");
    
    size_t length = array->elements ? fl_array_length(array->elements) : 0;
    if (length > 0)
    {
        for (size_t i=0; i < length; i++)
        {
            if (i > 0)
                fl_cstring_append(&output, ", ");

            struct ZirOperand *operand = array->elements[i];
            output = zir_operand_dump(operand, output);
        }
        fl_cstring_append(&output, " ");
    }

    fl_cstring_append(&output, "]");

    return output;
}

char* zir_operand_array_type_dump(struct ZirArrayOperand *array, char *output)
{
    fl_cstring_vappend(&output, "%s", zir_type_array_to_string(array->type));
    return output;
}

