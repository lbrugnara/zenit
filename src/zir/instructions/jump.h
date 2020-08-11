#ifndef ZIR_INSTRUCTION_JUMP_H
#define ZIR_INSTRUCTION_JUMP_H

#include "instruction.h"
#include "operands/operand.h"
#include "operands/symbol.h"
#include "../types/type.h"

/*
 * Struct: ZirJumpInstr
 *  The jump instruction changes the program's IP applying the *destination* operand as an offset
 * 
 * Members:
 *  <ZirInstr> base: Basic information of the instruction
 */
typedef struct ZirJumpInstr {
    ZirInstr base;
} ZirJumpInstr;

/*
 * Function: zir_jump_instr_new
 *  Creates and returns a new jump instruction
 *
 * Parameters:
 *  <ZirOperand> *destination: The destination operand of the jump instruction
 *
 * Returns:
 *  <ZirJumpInstr>*: Jump instruction object
 *
 * Notes:
 *  The object returned by this function must be freed with the
 *  <zir_jump_instr_free> function
 */
ZirJumpInstr* zir_jump_instr_new(ZirOperand *destination);

/*
 * Function: zir_jump_instr_free
 *  Releases the memory used by the jump instruction object
 *
 * Parameters:
 *  <ZirJumpInstr> *instruction: The jump instruction object to be freed
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_jump_instr_free(ZirJumpInstr *instruction);

/*
 * Function: zir_jump_instr_dump
 *  Dumps the string representation of the instruction to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_jump_instr_dump(instruction, output);
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
char* zir_jump_instr_dump(ZirJumpInstr *instruction, char *output);

#endif /* ZIR_INSTRUCTION_JUMP_H */
