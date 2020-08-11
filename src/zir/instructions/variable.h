#ifndef ZIR_INSTRUCTION_VARIABLE_H
#define ZIR_INSTRUCTION_VARIABLE_H

#include "instruction.h"
#include "operands/symbol.h"
#include "attributes/attribute-map.h"

/*
 * Struct: ZirVariableInstr
 *  Represents a variable declaration instruction
 * 
 * Members:
 *  <ZirInstr> base: Base information
 *  <ZirOperand> *source: Represents the right-hand side of the variable declaration
 *  <ZirAttributeMap> *attributes: Attributes of the variable declaration instruction
 * 
 */
typedef struct ZirVariableInstr {
    ZirInstr base;
    ZirOperand *source;
    ZirAttributeMap *attributes;
} ZirVariableInstr;

/*
 * Function: zir_variable_instr_new
 *  Creates a new variable instruction
 *
 * Parameters:
 *  <ZirOperand> *destination: The destination of the variable declaration instruction (it represents the var. decl. left-hand side)
 *  <ZirOperand> *source: The source operand of the variable declaration instruction (it represents the var. decl. right-hand side)
 *
 * Returns:
 *  <ZirVariableInstr>*: The variable declaration instruction
 *
 * Notes:
 *  The object returned by this function must be freed with the
 *  <zir_variable_instr_free> function
 */
ZirVariableInstr* zir_variable_instr_new(ZirOperand *destination, ZirOperand *source);

/*
 * Function: zir_variable_instr_free
 *  Releases the memory used by the variable declaration instruction
 *
 * Parameters:
 *  <ZirVariableInstr> *instruction: The instruction object to be freed
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_variable_instr_free(ZirVariableInstr *instruction);

/*
 * Function: zir_variable_instr_dump
 *  Dumps the string representation of the instruction to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_variable_instr_dump(instruction, output);
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
char* zir_variable_instr_dump(ZirVariableInstr *instruction, char *output);

#endif /* ZIR_INSTRUCTION_VARIABLE_H */
