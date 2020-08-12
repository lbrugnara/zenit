#ifndef ZNES_OPERAND_STRUCT_H
#define ZNES_OPERAND_STRUCT_H

#include "operand.h"

typedef struct ZnesStructOperandMember {
    const char *name;
    ZnesOperand *operand;
} ZnesStructOperandMember;

typedef struct ZnesStructOperand {
    ZnesOperand base;
    ZnesStructOperandMember **members;
} ZnesStructOperand;

ZnesStructOperand* znes_struct_operand_new(void);
void znes_struct_operand_add_member(ZnesStructOperand *struct_operand, const char *name, ZnesOperand *operand);
void znes_struct_operand_free(ZnesStructOperand *struct_operand);
size_t znes_struct_operand_size(ZnesStructOperand *struct_operand);
char* znes_struct_operand_dump(ZnesStructOperand *struct_operand, char *output);

#endif /* ZNES_OPERAND_STRUCT_H */
