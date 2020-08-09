#ifndef ZIR_INSTRUCTION_H
#define ZIR_INSTRUCTION_H

#include "operands/operand.h"

/*
 * Enum: ZirInstructionType
 *  Represents the different type of instructions available in ZIR
 * 
 */
typedef enum ZirInstructionType {
    ZIR_INSTR_VARIABLE,
    ZIR_INSTR_CAST,
} ZirInstructionType;

/*
 * Struct: ZirInstruction
 *  Base object that contains basic information between the
 *  different types of instructions
 * 
 * Members:
 *  <ZirInstructionType> type: Instruction's internal type
 *  <ZirOperand> *destination: The destination operand for the instruction's result
 * 
 */
typedef struct ZirInstruction {
    ZirInstructionType type;
    ZirOperand *destination;
} ZirInstruction;

/*
 * Function: zir_instruction_free
 *  Releases the memory used by an instruction object
 *
 * Parameters:
 *  <ZirInstruction> *instruction: The instruction object to be freed
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_instruction_free(ZirInstruction *instruction);

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
 *  <ZirInstruction> *instruction: Instruction object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  char*: *output* pointer
 *
 */
char* zir_instruction_dump(ZirInstruction *instruction, char *output);

#endif /* ZIR_INSTRUCTION_H */
