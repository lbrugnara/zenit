
#include <fllib/Mem.h>
#include <fllib/Array.h>
#include <fllib/Cstring.h>
#include "array.h"

ZnesArrayOperand* znes_array_operand_new(size_t element_size, size_t length)
{
    ZnesArrayOperand *operand = fl_malloc(sizeof(ZnesArrayOperand));
    operand->base.type = ZNES_OPERAND_ARRAY;
    operand->element_size = element_size;
    operand->length = length;
    operand->elements = fl_array_new(sizeof(ZnesOperand*), length);

    return operand;
}

void znes_array_operand_free(ZnesArrayOperand *array_operand)
{
    if (!array_operand)
        return;

    fl_array_free(array_operand->elements);

    fl_free(array_operand);
}

size_t znes_array_operand_size(ZnesArrayOperand *array_operand)
{
    // TODO: Handle overflow
    return array_operand->length * fl_array_length(array_operand->elements);
}

char* znes_array_operand_dump(ZnesArrayOperand *array, char *output)
{
    fl_cstring_append(&output, "[ ");
    
    size_t length = array->elements ? fl_array_length(array->elements) : 0;
    if (length > 0)
    {
        for (size_t i=0; i < length; i++)
        {
            if (i > 0)
                fl_cstring_append(&output, ", ");

            ZnesOperand *operand = array->elements[i];
            output = znes_operand_dump(operand, output);
        }
        fl_cstring_append(&output, " ");
    }

    fl_cstring_append(&output, "]");

    return output;
}
