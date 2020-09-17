#ifndef ZNES_OPERAND_H
#define ZNES_OPERAND_H

#include <stddef.h>

typedef enum ZnesOperandType {
    ZNES_OPERAND_UINT,
    ZNES_OPERAND_BOOL,
    ZNES_OPERAND_ARRAY,
    ZNES_OPERAND_STRUCT,
    ZNES_OPERAND_VARIABLE,
    ZNES_OPERAND_REFERENCE
} ZnesOperandType;

typedef struct ZnesOperand {
    ZnesOperandType type;
} ZnesOperand;

void znes_operand_free(ZnesOperand *operand);
size_t znes_operand_size(ZnesOperand *operand);
char* znes_operand_dump(ZnesOperand *operand, char *output);

#endif /* ZNES_OPERAND_H */
