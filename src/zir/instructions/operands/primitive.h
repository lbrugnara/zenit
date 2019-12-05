#ifndef ZIR_OPERAND_PRIMITIVE_H
#define ZIR_OPERAND_PRIMITIVE_H

#include "operand.h"
#include "primitive.h"

union ZirPrimitiveValue {
    uint8_t uint8;
    uint16_t uint16;
};

struct ZirPrimitiveOperand {
    struct ZirOperand base;
    union ZirPrimitiveValue value;
};

struct ZirPrimitiveOperand* zir_operand_primitive_new(enum ZirType type, union ZirPrimitiveValue value);
void zir_operand_primitive_free(struct ZirPrimitiveOperand *primitive);
char* zir_operand_primitive_dump(struct ZirPrimitiveOperand *operand, char *output);

#endif /* ZIR_OPERAND_PRIMITIVE_H */
