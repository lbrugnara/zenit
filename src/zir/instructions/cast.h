#ifndef ZIR_INSTRUCTION_CAST_H
#define ZIR_INSTRUCTION_CAST_H

#include "instruction.h"
#include "operands/operand.h"
#include "operands/symbol.h"
#include "../types/type.h"

/*
 * Struct: struct ZirCastInstruction
 *  The cast instruction "converts" the object from the *source* operand to a valid object for the
 *  *destination* operand
 * 
 * Members:
 *  <struct ZirInstruction> base: Basic information of the instruction
 *  <struct ZirOperand> *source: The source operand of the cast instruction
 */
struct ZirCastInstruction {
    struct ZirInstruction base;
    struct ZirOperand *source;
};

/*
 * Function: zir_instruction_cast_new
 *  Creates and returns a new cast instruction
 *
 * Parameters:
 *  <struct ZirOperand> *destination: The destination operand of the cast instruction
 *  <struct ZirOperand> *source: The source operand of the cast instruction
 *
 * Returns:
 *  <struct ZirCastInstruction>*: Cast instruction object
 *
 * Notes:
 *  The object returned by this function must be freed with the
 *  <zir_instruction_cast_free> function
 */
struct ZirCastInstruction* zir_instruction_cast_new(struct ZirOperand *destination, struct ZirOperand *source);

/*
 * Function: zir_instruction_cast_free
 *  Releases the memory used by the cast instruction object
 *
 * Parameters:
 *  <struct ZirCastInstruction> *instruction: The cast instruction object to be freed
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_instruction_cast_free(struct ZirCastInstruction *instruction);

/*
 * Function: zir_instruction_cast_dump
 *  Dumps the string representation of the instruction to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_instruction_cast_dump(instruction, output);
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
char* zir_instruction_cast_dump(struct ZirCastInstruction *instruction, char *output);

#endif /* ZIR_INSTRUCTION_CAST_H */
