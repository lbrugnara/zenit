#ifndef ZNES_JUMP_INSTR_H
#define ZNES_JUMP_INSTR_H

#include <fllib/Mem.h>
#include <fllib/containers/List.h>
#include "instr.h"
#include "../operands/uint.h"

typedef struct ZnesJumpInstruction {
    ZnesInstruction base;
    ZnesUintOperand *offset;   // Destination is a jump offset
} ZnesJumpInstruction;

ZnesJumpInstruction* znes_jump_instruction_new(ZnesUintOperand *offset);
void znes_jump_instruction_free(ZnesJumpInstruction *instruction);
char* znes_jump_instruction_dump(ZnesJumpInstruction *instruction, char *output);

#endif /* ZNES_JUMP_INSTR_H */
