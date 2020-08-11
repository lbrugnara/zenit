#ifndef ZIR_INSTRUCTION_H
#define ZIR_INSTRUCTION_H

#include "operands/operand.h"

/*
 * Enum: ZirInstrType
 *  Represents the different type of instructions available in ZIR
 * 
 */
typedef enum ZirInstrType {
    ZIR_INSTR_VARIABLE,
    ZIR_INSTR_IF_FALSE,
    ZIR_INSTR_CAST,
    ZIR_INSTR_JUMP,
} ZirInstrType;

/*
 * Struct: ZirInstr
 *  Base object that contains basic information between the
 *  different types of instructions
 * 
 * Members:
 *  <ZirInstrType> type: Instruction's internal type
 *  <ZirOperand> *destination: The destination operand for the instruction's result
 * 
 */
typedef struct ZirInstr {
    ZirInstrType type;
    ZirOperand *destination;
} ZirInstr;

/*
 * Function: zir_instruction_free
 *  Releases the memory used by an instruction object
 *
 * Parameters:
 *  <ZirInstr> *instruction: The instruction object to be freed
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_instruction_free(ZirInstr *instruction);

/*
 * Function: zir_instruction_dump
 *  Dumps the string representation of the instruction to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_instruction_dump(instruction, output);
 * ===========
 *
 * Parameters:
 *  <ZirInstr> *instruction: Instruction object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  char*: *output* pointer
 *
 */
char* zir_instruction_dump(ZirInstr *instruction, char *output);

#endif /* ZIR_INSTRUCTION_H */
