#ifndef ZNES_OPERAND_UINT_H
#define ZNES_OPERAND_UINT_H

#include <stdint.h>
#include "operand.h"

typedef enum ZnesUintSize {
    ZNES_UINT_UNK,
    ZNES_UINT_8,
    ZNES_UINT_16
} ZnesUintSize;

typedef union ZnesUintValue {
    uint8_t uint8;
    uint16_t uint16;
} ZnesUintValue;

typedef struct ZnesUintOperand {
    ZnesOperand base;
    ZnesUintSize size;
    ZnesUintValue value;
} ZnesUintOperand;

ZnesUintOperand* znes_uint_operand_new(ZnesUintSize size, ZnesUintValue value);
void znes_uint_operand_free(ZnesUintOperand *uint_operand);
char* znes_uint_operand_dump(ZnesUintOperand *uint_operand, char *output);

static inline size_t znes_uint_operand_size(ZnesUintOperand *uint_operand)
{
    return 1; // byte
}

#endif /* ZNES_OPERAND_UINT_H */
