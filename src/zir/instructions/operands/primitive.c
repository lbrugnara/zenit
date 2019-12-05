#include <fllib.h>
#include "primitive.h"

struct ZirPrimitiveOperand* zir_operand_primitive_new(enum ZirType type, union ZirPrimitiveValue value)
{
    struct ZirPrimitiveOperand *primitive = fl_malloc(sizeof(struct ZirPrimitiveOperand));
    primitive->base.type = ZIR_OPERAND_PRIMITIVE;
    
    primitive->value = value;

    return primitive;
}

void zir_operand_primitive_free(struct ZirPrimitiveOperand *primitive)
{
    if (!primitive)
        return;

    zir_type_free(&primitive->base.typeinfo);

    fl_free(primitive);
}

char* zir_operand_primitive_dump(struct ZirPrimitiveOperand *literal, char *output)
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
