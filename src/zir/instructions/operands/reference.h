#ifndef ZIR_OPERAND_REFERENCE_H
#define ZIR_OPERAND_REFERENCE_H

#include "operand.h"
#include "symbol.h"

struct ZirReferenceOperand {
    struct ZirOperand base;
    struct ZirReferenceType *type;
    struct ZirSymbolOperand *operand;
};

struct ZirReferenceOperand* zir_operand_reference_new(struct ZirReferenceType *type, struct ZirSymbolOperand *operand);
void zir_operand_reference_free(struct ZirReferenceOperand *reference);
char* zir_operand_reference_dump(struct ZirReferenceOperand *operand, char *output);
char* zir_operand_reference_type_dump(struct ZirReferenceOperand *reference, char *output);

#endif /* ZIR_OPERAND_REFERENCE_H */
