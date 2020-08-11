
#include <fllib/Cstring.h>
#include "uint.h"
#include "../../types/uint.h"

ZirUintOperand* zir_uint_operand_new(ZirUintType *type, ZirUintValue value)
{
    ZirUintOperand *uint = fl_malloc(sizeof(ZirUintOperand));
    uint->base.type = ZIR_OPERAND_UINT;
    uint->value = value;
    uint->type = type;

    return uint;
}

void zir_uint_operand_free(ZirUintOperand *uint)
{
    if (!uint)
        return;

    if (uint->type)
        zir_uint_type_free(uint->type);

    fl_free(uint);
}

char* zir_uint_operand_dump(ZirUintOperand *uint, char *output)
{
    switch (uint->type->size)
    {
        case ZIR_UINT_8:
            fl_cstring_vappend(&output, "%u", uint->value.uint8);
            break;

        case ZIR_UINT_16:
            fl_cstring_vappend(&output, "%u", uint->value.uint16);
            break;

        case ZIR_UINT_UNK:
            output = *fl_cstring_append(&output, "<unknown uint>");
            break;

        default:
            output = *fl_cstring_append(&output, "<error>");
            break;
    }

    return output;
}

char* zir_uint_operand_type_dump(ZirUintOperand *uint, char *output)
{
    fl_cstring_vappend(&output, "%s", zir_uint_type_to_string(uint->type));
    return output;
}
