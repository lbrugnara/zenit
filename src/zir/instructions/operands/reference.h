#ifndef ZIR_OPERAND_REFERENCE_H
#define ZIR_OPERAND_REFERENCE_H

#include "operand.h"

struct ZirReferenceOperand {
    struct ZirOperand base;
    struct ZirOperand *operand;
};

struct ZirReferenceOperand* zir_operand_reference_new(struct ZirOperand *operand);
void zir_operand_reference_free(struct ZirReferenceOperand *reference);
char* zir_operand_reference_dump(struct ZirReferenceOperand *operand, char *output);

#endif /* ZIR_OPERAND_REFERENCE_H */
