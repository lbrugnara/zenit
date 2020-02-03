#ifndef ZIR_INSTRUCTION_VARIABLE_H
#define ZIR_INSTRUCTION_VARIABLE_H

#include "instruction.h"
#include "operands/symbol.h"
#include "attributes/attribute_map.h"

/*
 * Struct: struct ZirVariableInstruction
 *  Represents a variable declaration instruction
 * 
 * Members:
 *  <struct ZirInstruction> base: Base information
 *  <struct ZirOperand> *source: Represents the right-hand side of the variable declaration
 *  <ZirAttributeMap> *attributes: Attributes of the variable declaration instruction
 * 
 */
struct ZirVariableInstruction {
    struct ZirInstruction base;
    struct ZirOperand *source;
    ZirAttributeMap *attributes;
};

/*
 * Function: zir_instruction_variable_new
 *  Creates a new variable instruction
 *
 * Parameters:
 *  <struct ZirOperand> *destination: The destination of the variable declaration instruction (it represents the var. decl. left-hand side)
 *  <struct ZirOperand> *source: The source operand of the variable declaration instruction (it represents the var. decl. right-hand side)
 *
 * Returns:
 *  <struct ZirVariableInstruction>*: The variable declaration instruction
 *
 * Notes:
 *  The object returned by this function must be freed with the
 *  <zir_instruction_variable_free> function
 */
struct ZirVariableInstruction* zir_instruction_variable_new(struct ZirOperand *destination, struct ZirOperand *source);

/*
 * Function: zir_instruction_variable_free
 *  Releases the memory used by the variable declaration instruction
 *
 * Parameters:
 *  <struct ZirVariableInstruction> *instruction: The instruction object to be freed
 *
 * Returns:
 *  void: This function does not return a value
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
 *  instruction: Instruction object
 *  output: Output buffer
 *
 * Returns:
 *  char*: *output* pointer
 *
 */
char* zir_instruction_variable_dump(struct ZirVariableInstruction *instruction, char *output);

#endif /* ZIR_INSTRUCTION_VARIABLE_H */
