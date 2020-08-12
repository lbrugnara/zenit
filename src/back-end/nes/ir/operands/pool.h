#ifndef ZNES_OPERAND_POOL_H
#define ZNES_OPERAND_POOL_H


#include "array.h"
#include "bool.h"
#include "reference.h"
#include "struct.h"
#include "variable.h"
#include "uint.h"

typedef struct ZnesOperandPool {
    FlList *operands;
} ZnesOperandPool;

ZnesOperandPool* znes_operand_pool_new(void);
void znes_operand_pool_free(ZnesOperandPool *pool);
void znes_operand_pool_register(ZnesOperandPool *pool, ZnesOperand *operand);

#endif /* ZNES_OPERAND_POOL_H */
