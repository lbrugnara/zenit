#ifndef ZIR_OPERAND_POOL_H
#define ZIR_OPERAND_POOL_H

#include <fllib.h>
#include "array.h"
#include "reference.h"
#include "symbol.h"
#include "uint.h"

struct ZirOperandPool {
    FlList operands;
};

struct ZirOperandPool* zir_operand_pool_new(void);
void zir_operand_pool_free(struct ZirOperandPool *pool);
struct ZirArrayOperand* zir_operand_pool_new_array(struct ZirOperandPool *pool, struct ZirArrayType *type);
struct ZirReferenceOperand* zir_operand_pool_new_reference(struct ZirOperandPool *pool, struct ZirReferenceType *type, struct ZirSymbolOperand *operand);
struct ZirSymbolOperand* zir_operand_pool_new_symbol(struct ZirOperandPool *pool, struct ZirSymbol *symbol);
struct ZirUintOperand* zir_operand_pool_new_uint(struct ZirOperandPool *pool, struct ZirUintType *type, union ZirUintValue value);

#endif /* ZIR_OPERAND_POOL_H */
