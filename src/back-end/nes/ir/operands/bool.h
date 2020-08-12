#ifndef ZNES_OPERAND_BOOL_H
#define ZNES_OPERAND_BOOL_H

#include <stdbool.h>
#include "operand.h"

typedef struct ZnesBoolOperand {
    ZnesOperand base;
    bool value;
} ZnesBoolOperand;

ZnesBoolOperand* znes_bool_operand_new(bool value);
void znes_bool_operand_free(ZnesBoolOperand *bool_operand);
char* znes_bool_operand_dump(ZnesBoolOperand *bool_operand, char *output);

static inline size_t znes_bool_operand_size(ZnesBoolOperand *bool_operand)
{
    return 1; // byte
}

#endif /* ZNES_OPERAND_BOOL_H */
