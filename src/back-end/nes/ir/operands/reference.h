#ifndef ZNES_OPERAND_REFERENCE_H
#define ZNES_OPERAND_REFERENCE_H

#include "operand.h"
#include "variable.h"

typedef struct ZnesReferenceOperand {
    ZnesOperand base;
    ZnesVariableOperand *operand;
} ZnesReferenceOperand;

ZnesReferenceOperand* znes_reference_operand_new(ZnesVariableOperand *operand);
void znes_reference_operand_free(ZnesReferenceOperand *reference_operand);
char* znes_reference_operand_dump(ZnesReferenceOperand *reference_operand, char *output);

static inline size_t znes_reference_operand_size(ZnesReferenceOperand *reference_operand)
{
    return 2; // bytes
}

#endif /* ZNES_OPERAND_REFERENCE_H */
