#ifndef ZIR_INSTRUCTION_IF_TRUE_H
#define ZIR_INSTRUCTION_IF_TRUE_H

#include "instruction.h"
#include "operands/operand.h"
#include "operands/symbol.h"
#include "../types/type.h"

/*
 * Struct: ZirIfFalseInstr
 *  The if-false instruction uses the *destination* operand to calculate the next jump if the 
 *  *source* operand is a non-truthy value. Otherwise, the execution of the program must flow normally
 * 
 * Members:
 *  <ZirInstr> base: Basic information of the instruction
 *  <ZirOperand> *source: The source operand of the if-false instruction
 */
typedef struct ZirIfFalseInstr {
    ZirInstr base;
    ZirOperand *source;
} ZirIfFalseInstr;

/*
 * Function: zir_if_false_instr_new
 *  Creates and returns a new if-false instruction
 *
 * Parameters:
 *  <ZirOperand> *destination: The destination operand of the if-false instruction
 *  <ZirOperand> *source: The source operand of the if-false instruction
 *
 * Returns:
 *  <ZirIfFalseInstr>*: If-true instruction object
 *
 * Notes:
 *  The object returned by this function must be freed with the
 *  <zir_if_false_instr_free> function
 */
ZirIfFalseInstr* zir_if_false_instr_new(ZirOperand *destination, ZirOperand *source);

/*
 * Function: zir_if_false_instr_free
 *  Releases the memory used by the if-false instruction object
 *
 * Parameters:
 *  <ZirIfFalseInstr> *instruction: The if-false instruction object to be freed
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_if_false_instr_free(ZirIfFalseInstr *instruction);

/*
 * Function: zir_if_false_instr_dump
 *  Dumps the string representation of the instruction to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_if_false_instr_dump(instruction, output);
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
char* zir_if_false_instr_dump(ZirIfFalseInstr *instruction, char *output);

#endif /* ZIR_INSTRUCTION_IF_TRUE_H */
