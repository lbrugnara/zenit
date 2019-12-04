#ifndef ZIR_INSTRUCTION_H
#define ZIR_INSTRUCTION_H

#include "../operand.h"
#include "../attribute.h"

/*
 * Enum: enum ZirInstructionType
 *  Represents the different type of instructions available in ZIR
 * 
 */
enum ZirInstructionType {
    ZIR_INSTR_VARIABLE,
    ZIR_INSTR_CAST,
};

/*
 * Struct: struct ZirInstruction
 *  Base object that tracks common information between the
 *  different types of instructions
 * 
 * Members:
 *  <enum ZirInstructionType> type: Instruction's internal type
 * 
 */
struct ZirInstruction {
    enum ZirInstructionType type;
};

/*
 * Function: zir_instruction_free
 *  Releases the memory used by an instruction object
 *
 * Parameters:
 *  <struct ZirInstruction> *instruction - The instruction object to be freed
 *
 * Returns:
 *  void - This function does not return a value
 *
 */
void zir_instruction_free(struct ZirInstruction *instruction);

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
 *  instruction - Instruction object
 *  output - Output buffer
 *
 * Returns:
 *  char* - *output* pointer
 *
 */
char* zir_instruction_dump(struct ZirInstruction *instruction, char *output);

#endif /* ZIR_INSTRUCTION_H */
