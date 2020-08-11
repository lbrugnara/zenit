#ifndef ZIR_INSTRUCTION_JUMP_H
#define ZIR_INSTRUCTION_JUMP_H

#include "instruction.h"
#include "operands/operand.h"
#include "operands/symbol.h"
#include "../types/type.h"

/*
 * Struct: ZirJumpInstruction
 *  The jump instruction changes the program's IP applying the *destination* operand as an offset
 * 
 * Members:
 *  <ZirInstruction> base: Basic information of the instruction
 */
typedef struct ZirJumpInstruction {
    ZirInstruction base;
} ZirJumpInstruction;

/*
 * Function: zir_instruction_jump_new
 *  Creates and returns a new jump instruction
 *
 * Parameters:
 *  <ZirOperand> *destination: The destination operand of the jump instruction
 *
 * Returns:
 *  <ZirJumpInstruction>*: Jump instruction object
 *
 * Notes:
 *  The object returned by this function must be freed with the
 *  <zir_instruction_jump_free> function
 */
ZirJumpInstruction* zir_instruction_jump_new(ZirOperand *destination);

/*
 * Function: zir_instruction_jump_free
 *  Releases the memory used by the jump instruction object
 *
 * Parameters:
 *  <ZirJumpInstruction> *instruction: The jump instruction object to be freed
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_instruction_jump_free(ZirJumpInstruction *instruction);

/*
 * Function: zir_instruction_jump_dump
 *  Dumps the string representation of the instruction to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_instruction_jump_dump(instruction, output);
 * ===========
 *
 * Parameters:
 *  instruction: Instruction object
 *  output: Output buffer
 *
 * Returns:
 *  char*: *output* pointer
 *
 */
char* zir_instruction_jump_dump(ZirJumpInstruction *instruction, char *output);

#endif /* ZIR_INSTRUCTION_JUMP_H */
