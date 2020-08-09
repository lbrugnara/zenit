
#include <fllib/Mem.h>
#include <fllib/Array.h>
#include <fllib/Cstring.h>
#include "array.h"
#include "../../types/array.h"

ZirArrayOperand* zir_operand_array_new(ZirArrayType *type)
{
    ZirArrayOperand *operand = fl_malloc(sizeof(ZirArrayOperand));
    operand->base.type = ZIR_OPERAND_ARRAY;
    operand->elements = fl_array_new(sizeof(ZirOperand*), 0);
    operand->type = type;

    return operand;
}

void zir_operand_array_add_element(ZirArrayOperand *array_operand, ZirOperand *member_operand)
{
    array_operand->elements = fl_array_append(array_operand->elements, &member_operand);
}

void zir_operand_array_free(ZirArrayOperand *array_operand)
{
    if (!array_operand)
        return;

    fl_array_free(array_operand->elements);

    if (array_operand->type)
        zir_type_array_free(array_operand->type);

    fl_free(array_operand);
}

char* zir_operand_array_dump(ZirArrayOperand *array, char *output)
{
    fl_cstring_append(&output, "[ ");
    
    size_t length = array->elements ? fl_array_length(array->elements) : 0;
    if (length > 0)
    {
        for (size_t i=0; i < length; i++)
        {
            if (i > 0)
                fl_cstring_append(&output, ", ");

            ZirOperand *operand = array->elements[i];
            output = zir_operand_dump(operand, output);
        }
        fl_cstring_append(&output, " ");
    }

    fl_cstring_append(&output, "]");

    return output;
}

char* zir_operand_array_type_dump(ZirArrayOperand *array, char *output)
{
    fl_cstring_vappend(&output, "%s", zir_type_array_to_string(array->type));
    return output;
}

