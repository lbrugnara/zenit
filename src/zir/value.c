#include "value.h"


static inline char* dump_literal(struct ZirLiteralValue *literal, char *output)
{
    switch (literal->base.typeinfo.type)
    {
        case ZIR_TYPE_UINT8:
            fl_cstring_vappend(&output, "%u", literal->value.uint8);
            break;
        case ZIR_TYPE_UINT16:
            fl_cstring_vappend(&output, "%u", literal->value.uint16);
            break;
        case ZIR_TYPE_NONE:
            output = *fl_cstring_append(&output, "<none>");
            break;
        case ZIR_TYPE_CUSTOM:
            output = *fl_cstring_append(&output, "<error>");
            break;

        default:
            output = *fl_cstring_append(&output, "<error>");
            break;
    }

    return output;
}

static inline char* dump_array(struct ZirArrayValue *array, char *output)
{
    fl_cstring_append(&output, "[ ");
    if (array->elements)
    {
        for (size_t i=0; i < fl_array_length(array->elements); i++)
        {
            if (i > 0)
                fl_cstring_append(&output, ", ");

            struct ZirOperand *operand = &array->elements[i];
            output = zir_operand_dump(operand, output);
        }
    }
    fl_cstring_append(&output, " ]");

    return output;
}

/* Public API */

struct ZirValue* zir_value_new(enum ZirValueType type)
{
    struct ZirValue *value = NULL;
    
    switch (type)
    {
        case ZIR_VALUE_LITERAL:
            value = fl_malloc(sizeof(struct ZirLiteralValue));
            break;
        case ZIR_VALUE_ARRAY:
            value = fl_malloc(sizeof(struct ZirArrayValue));
            break;
        default:
            return NULL;
    }

    value->type = type;

    return value;
}

void zir_value_free(struct ZirValue *value)
{
    switch (value->type)
    {
        case ZIR_VALUE_LITERAL:
        {
            // struct ZirLiteralValue *literal = (struct ZirLiteralValue*)value;
            // Nothing particular to free here
            break;
        }
        case ZIR_VALUE_ARRAY:
        {
            struct ZirArrayValue *array = (struct ZirArrayValue*)value;
            if (array->elements)
            {
                for (size_t i=0; i < fl_array_length(array->elements); i++)
                    zir_operand_free(&array->elements[i]);

                fl_array_free(array->elements);
            }
            break;
        }
    }

    if (value->typeinfo.name)
        fl_cstring_free(value->typeinfo.name);

    fl_free(value);
}

char* zir_value_dump(struct ZirValue *value, char *output)
{
    switch (value->type)
    {
        case ZIR_VALUE_LITERAL:
            output = dump_literal((struct ZirLiteralValue*)value, output);
            break;
        case ZIR_VALUE_ARRAY:
            output = dump_array((struct ZirArrayValue*)value, output);
            break;
    }

    return output;
}
