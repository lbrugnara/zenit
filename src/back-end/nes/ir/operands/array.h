#ifndef ZNES_OPERAND_ARRAY_H
#define ZNES_OPERAND_ARRAY_H

#include "operand.h"

typedef struct ZnesArrayOperand {
    ZnesOperand base;
    ZnesOperand **elements;
    size_t element_size;
    size_t length;
} ZnesArrayOperand;

ZnesArrayOperand* znes_array_operand_new(size_t element_size, size_t length);
void znes_array_operand_free(ZnesArrayOperand *array_operand);
char* znes_array_operand_dump(ZnesArrayOperand *array_operand, char *output);
size_t znes_array_operand_size(ZnesArrayOperand *array_operand);

#endif /* ZNES_OPERAND_ARRAY_H */
