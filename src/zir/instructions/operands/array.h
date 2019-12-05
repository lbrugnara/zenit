#ifndef ZIR_OPERAND_ARRAY_H
#define ZIR_OPERAND_ARRAY_H

#include "operand.h"
#include "array.h"

struct ZirArrayOperand {
    struct ZirOperand base;
    struct ZirOperand **elements;
};

struct ZirArrayOperand* zir_operand_array_new(void);
void zir_operand_array_free(struct ZirArrayOperand *operand);
char* zir_operand_array_dump(struct ZirArrayOperand *operand, char *output);

#endif /* ZIR_OPERAND_ARRAY_H */
