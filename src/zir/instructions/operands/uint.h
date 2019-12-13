#ifndef ZIR_OPERAND_UINT_H
#define ZIR_OPERAND_UINT_H

#include <stdint.h>
#include "operand.h"
#include "uint.h"

union ZirUintValue {
    uint8_t uint8;
    uint16_t uint16;
};

struct ZirUintOperand {
    struct ZirOperand base;
    struct ZirUintTypeInfo *typeinfo;
    union ZirUintValue value;
};

struct ZirUintOperand* zir_operand_uint_new(struct ZirUintTypeInfo *typeinfo, union ZirUintValue value);
void zir_operand_uint_free(struct ZirUintOperand *primitive);
char* zir_operand_uint_dump(struct ZirUintOperand *uint, char *output);
char* zir_operand_uint_type_dump(struct ZirUintOperand *uint, char *output);

#endif /* ZIR_OPERAND_UINT_H */
