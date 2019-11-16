#include "value.h"


static inline char* dump_literal(struct ZenitIrLiteralValue *literal, char *output)
{
    switch (literal->base.typeinfo.type)
    {
        case ZENIT_IR_TYPE_UINT8:
            fl_cstring_vappend(&output, "%u", literal->value.uint8);
            break;
        case ZENIT_IR_TYPE_UINT16:
            fl_cstring_vappend(&output, "%u", literal->value.uint16);
            break;
        case ZENIT_IR_TYPE_NONE:
            output = *fl_cstring_append(&output, "<none>");
            break;
        case ZENIT_IR_TYPE_CUSTOM:
            output = *fl_cstring_append(&output, "<error>");
            break;
    }

    return output;
}

static inline char* dump_array(struct ZenitIrArrayValue *array, char *output)
{
    fl_cstring_append(&output, "[ ");
    if (array->elements)
    {
        for (size_t i=0; i < fl_array_length(array->elements); i++)
        {
            if (i > 0)
                fl_cstring_append(&output, ", ");

            struct ZenitIrOperand *operand = &array->elements[i];
            output = zenit_ir_operand_dump(operand, output);
        }
    }
    fl_cstring_append(&output, " ]");

    return output;
}

/* Public API */

struct ZenitIrValue* zenit_ir_value_new(enum ZenitIrValueType type)
{
    struct ZenitIrValue *value = NULL;
    
    switch (type)
    {
        case ZENIT_IR_VALUE_LITERAL:
            value = fl_malloc(sizeof(struct ZenitIrLiteralValue));
            break;
        case ZENIT_IR_VALUE_ARRAY:
            value = fl_malloc(sizeof(struct ZenitIrArrayValue));
            break;
        default:
            return NULL;
    }

    value->type = type;

    return value;
}

void zenit_ir_value_free(struct ZenitIrValue *value)
{
    switch (value->type)
    {
        case ZENIT_IR_VALUE_LITERAL:
            // struct ZenitIrLiteralValue *literal = (struct ZenitIrLiteralValue*)value;
            // Nothing particular to free here
            break;
        case ZENIT_IR_VALUE_ARRAY:
            struct ZenitIrArrayValue *array = (struct ZenitIrArrayValue*)value;
            if (array->elements)
            {
                for (size_t i=0; i < fl_array_length(array->elements); i++)
                    zenit_ir_operand_free(&array->elements[i]);

                fl_array_free(array->elements);
            }
            break;
    }

    if (value->typeinfo.name)
        fl_cstring_free(value->typeinfo.name);

    fl_free(value);
}

char* zenit_ir_value_dump(struct ZenitIrValue *value, char *output)
{
    switch (value->type)
    {
        case ZENIT_IR_VALUE_LITERAL:
            output = dump_literal((struct ZenitIrLiteralValue*)value, output);
            break;
        case ZENIT_IR_VALUE_ARRAY:
            output = dump_array((struct ZenitIrArrayValue*)value, output);
            break;
    }

    return output;
}
