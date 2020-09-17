#ifndef ZNES_IF_FALSE_INSTR_H
#define ZNES_IF_FALSE_INSTR_H

#include <fllib/Mem.h>
#include <fllib/containers/List.h>
#include "instr.h"
#include "../operands/uint.h"

typedef struct ZnesIfFalseInstruction {
    ZnesInstruction base;
    ZnesUintOperand *offset;   // Destination is a jump offset
    ZnesOperand *source;
} ZnesIfFalseInstruction;

ZnesIfFalseInstruction* znes_if_false_instruction_new(ZnesUintOperand *offset, ZnesOperand *source);
void znes_if_false_instruction_free(ZnesIfFalseInstruction *instruction);
char* znes_if_false_instruction_dump(ZnesIfFalseInstruction *instruction, char *output);

#endif /* ZNES_IF_FALSE_INSTR_H */
