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

void zir_operand_array_add_member(struct ZirArrayOperand *array_operand, struct ZirOperand *member_operand)
{
    if (member_operand->owner == NULL)
        member_operand->owner = array_operand;

    array_operand->elements = fl_array_append(array_operand->elements, &member_operand);
}

void zir_operand_array_free(struct ZirArrayOperand *array_operand)
{
    if (!array_operand)
        return;

    for (size_t i=0; i < fl_array_length(array_operand->elements); i++)
    {
        if (array_operand->elements[i]->owner == array_operand)
            zir_operand_free(array_operand->elements[i]);
    }

    fl_array_free(array_operand->elements);

    if (array_operand->type)
        zir_type_array_free(array_operand->type);

    fl_free(array_operand);
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

