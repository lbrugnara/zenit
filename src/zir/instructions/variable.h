#ifndef ZIR_VARIABLE_H
#define ZIR_VARIABLE_H

#include "instruction.h"
#include "../operand.h"
#include "../attribute.h"

/*
 * Struct: struct ZirVariableInstruction
 *  Represents a variable declaration instruction
 * 
 * Members:
 *  <struct ZirInstruction> base: Base information
 *  <struct ZirOperand> lvalue: Left-hand side of the variable declaration
 *  <struct ZirOperand> rvalue: Right-hand side of the variable declaration
 * 
 */
struct ZirVariableInstruction {
    struct ZirInstruction base;
    struct ZirAttributeMap attributes;
    struct ZirOperand lvalue;
    struct ZirOperand rvalue;
};

/*
 * Function: zir_instruction_variable_new
 *  Creates and returns a new instruction based on the provided *type*
 *
 * Parameters:
 *  <enum ZirInstructionType> type - Type of the instruction to be created
 *
 * Returns:
 *  <struct ZirInstruction>* - Instruction object
 *
 * Notes:
 *  The object returned by this function must be freed with the
 *  <zir_instruction_variable_free> function
 */
struct ZirVariableInstruction* zir_instruction_variable_new(void);

/*
 * Function: zir_instruction_variable_free
 *  Releases the memory used by an instruction object
 *
 * Parameters:
 *  <struct ZirInstruction> *instruction - The instruction object to be freed
 *
 * Returns:
 *  void - This function does not return a value
 *
 */
void zir_instruction_variable_free(struct ZirVariableInstruction *instruction);

/*
 * Function: zir_instruction_variable_dump
 *  Dumps the string representation of the instruction to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_instruction_variable_dump(instruction, output);
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
char* zir_instruction_variable_dump(struct ZirVariableInstruction *instruction, char *output);

#endif /* ZIR_VARIABLE_H */
