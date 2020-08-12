
#include <fllib/Cstring.h>
#include "uint.h"

ZnesUintOperand* znes_uint_operand_new(ZnesUintSize size, ZnesUintValue value)
{
    ZnesUintOperand *uint = fl_malloc(sizeof(ZnesUintOperand));
    uint->base.type = ZNES_OPERAND_UINT;
    uint->size = size;
    uint->value = value;

    return uint;
}

void znes_uint_operand_free(ZnesUintOperand *uint)
{
    if (!uint)
        return;

    fl_free(uint);
}

char* znes_uint_operand_dump(ZnesUintOperand *uint, char *output)
{
    switch (uint->size)
    {
        case ZNES_UINT_8:
            fl_cstring_vappend(&output, "%u", uint->value.uint8);
            break;

        case ZNES_UINT_16:
            fl_cstring_vappend(&output, "%u", uint->value.uint16);
            break;

        case ZNES_UINT_UNK:
            output = *fl_cstring_append(&output, "<unknown uint>");
            break;

        default:
            output = *fl_cstring_append(&output, "<error>");
            break;
    }

    return output;
}
