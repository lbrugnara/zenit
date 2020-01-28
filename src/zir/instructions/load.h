#ifndef ZIR_INSTRUCTION_LOAD_H
#define ZIR_INSTRUCTION_LOAD_H

#include "instruction.h"
#include "operands/operand.h"
#include "operands/symbol.h"
#include "../types/type.h"

/*
 * Struct: struct ZirLoadInstruction
 *  The load instruction "prepares" the destination operand to be used by another instruction
 *  as the source operand
 * 
 * Members:
 *  <struct ZirInstruction> base: Basic information of the instruction
 */
struct ZirLoadInstruction {
    struct ZirInstruction base;
};

/*
 * Function: zir_instruction_load_new
 *  Creates and returns a new load instruction
 *
 * Parameters:
 *  <struct ZirOperand> *destination: The destination operand of the load instruction
 *
 * Returns:
 *  <struct ZirLoadInstruction>*: Load instruction object
 *
 * Notes:
 *  The object returned by this function must be freed with the
 *  <zir_instruction_load_free> function
 */
struct ZirLoadInstruction* zir_instruction_load_new(struct ZirOperand *destination);

/*
 * Function: zir_instruction_load_free
 *  Releases the memory used by the load instruction object
 *
 * Parameters:
 *  <struct ZirLoadInstruction> *instruction: The load instruction object to be freed
 *
 * Returns:
 *  void: This function does not return a value
 */
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
