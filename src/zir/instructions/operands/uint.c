#include <fllib.h>
#include "uint.h"
#include "../../types/uint.h"

struct ZirUintOperand* zir_operand_uint_new(struct ZirUintTypeInfo *typeinfo, union ZirUintValue value)
{
    struct ZirUintOperand *uint = fl_malloc(sizeof(struct ZirUintOperand));
    uint->base.type = ZIR_OPERAND_UINT;
    uint->value = value;
    uint->typeinfo = typeinfo;

    return uint;
}

void zir_operand_uint_free(struct ZirUintOperand *uint)
{
    if (!uint)
        return;

    if (uint->typeinfo)
        zir_type_uint_free(uint->typeinfo);

    fl_free(uint);
}

char* zir_operand_uint_dump(struct ZirUintOperand *uint, char *output)
{
    switch (uint->typeinfo->size)
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

char* zir_operand_uint_type_dump(struct ZirUintOperand *uint, char *output)
{
    fl_cstring_vappend(&output, "%s", zir_type_uint_to_string(uint->typeinfo));
    return output;
}
