#ifndef ZIR_INSTRUCTION_CAST_H
#define ZIR_INSTRUCTION_CAST_H

#include "instruction.h"
#include "operands/operand.h"
#include "operands/symbol.h"
#include "../types/type.h"

/*
 * Struct: ZirCastInstr
 *  The cast instruction "converts" the object from the *source* operand to a valid object for the
 *  *destination* operand
 * 
 * Members:
 *  <ZirInstr> base: Basic information of the instruction
 *  <ZirOperand> *source: The source operand of the cast instruction
 */
typedef struct ZirCastInstr {
    ZirInstr base;
    ZirOperand *source;
} ZirCastInstr;

/*
 * Function: zir_cast_instr_new
 *  Creates and returns a new cast instruction
 *
 * Parameters:
 *  <ZirOperand> *destination: The destination operand of the cast instruction
 *  <ZirOperand> *source: The source operand of the cast instruction
 *
 * Returns:
 *  <ZirCastInstr>*: Cast instruction object
 *
 * Notes:
 *  The object returned by this function must be freed with the
 *  <zir_cast_instr_free> function
 */
ZirCastInstr* zir_cast_instr_new(ZirOperand *destination, ZirOperand *source);

/*
 * Function: zir_cast_instr_free
 *  Releases the memory used by the cast instruction object
 *
 * Parameters:
 *  <ZirCastInstr> *instruction: The cast instruction object to be freed
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_cast_instr_free(ZirCastInstr *instruction);

/*
 * Function: zir_cast_instr_dump
 *  Dumps the string representation of the instruction to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_cast_instr_dump(instruction, output);
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
char* zir_cast_instr_dump(ZirCastInstr *instruction, char *output);

#endif /* ZIR_INSTRUCTION_CAST_H */
