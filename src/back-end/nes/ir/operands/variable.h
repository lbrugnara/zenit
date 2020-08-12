#ifndef ZNES_OPERAND_VARIABLE_H
#define ZNES_OPERAND_VARIABLE_H

#include "operand.h"
#include "../objects/alloc.h"

typedef struct ZnesVariableOperand {
    ZnesOperand base;
    ZnesAlloc *variable;
} ZnesVariableOperand;

ZnesVariableOperand* znes_variable_operand_new(ZnesAlloc *variable);
void znes_variable_operand_free(ZnesVariableOperand *variable_operand);
char* znes_variable_operand_dump(ZnesVariableOperand *variable_operand, char *output);

static inline size_t znes_variable_operand_size(ZnesVariableOperand *variable_operand)
{
    return variable_operand->variable->size;
}

#endif /* ZNES_OPERAND_VARIABLE_H */
