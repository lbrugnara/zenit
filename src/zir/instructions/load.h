#ifndef ZIR_INSTRUCTION_LOAD_H
#define ZIR_INSTRUCTION_LOAD_H

#include "instruction.h"
#include "operands/operand.h"
#include "operands/symbol.h"
#include "../type.h"

struct ZirLoadInstruction {
    struct ZirInstruction base;
};

struct ZirLoadInstruction* zir_instruction_load_new(struct ZirOperand *destination);
void zir_instruction_load_free(struct ZirLoadInstruction *instruction);

/*
 * Function: zir_instruction_load_dump
 *  Dumps the string representation of the instruction to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_instruction_load_dump(instruction, output);
 * ===========
 *
 * Parameters:
 *  instruction - Instruction object
 *  output - Output buffer
 *
 * Returns:
 *  char* - *output* pointer
 *
 */
char* zir_instruction_load_dump(struct ZirLoadInstruction *instruction, char *output);

#endif /* ZIR_INSTRUCTION_LOAD_H */
