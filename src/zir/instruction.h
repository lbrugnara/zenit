#ifndef ZENIT_IR_INSTRUCTION_H
#define ZENIT_IR_INSTRUCTION_H

#include "operand.h"
#include "attribute.h"

/*
 * Enum: enum ZenitIrInstructionType
 *  Represents the different type of instructions available in ZIR
 * 
 */
enum ZenitIrInstructionType {
    ZENIT_IR_INSTR_VARIABLE,
};

/*
 * Struct: struct ZenitIrInstruction
 *  Base object that tracks common information between the
 *  different types of instructions
 * 
 * Members:
 *  <enum ZenitIrInstructionType> type: Instruction's internal type
 * 
 */
struct ZenitIrInstruction {
    enum ZenitIrInstructionType type;
};

/*
 * Struct: struct ZenitIrVariableInstruction
 *  Represents a variable declaration instruction
 * 
 * Members:
 *  <struct ZenitIrInstruction> base: Base information
 *  <struct ZenitIrOperand> lvalue: Left-hand side of the variable declaration
 *  <struct ZenitIrOperand> rvalue: Right-hand side of the variable declaration
 * 
 */
struct ZenitIrVariableInstruction {
    struct ZenitIrInstruction base;
    struct ZenitIrAttributeMap attributes;
    struct ZenitIrOperand lvalue;
    struct ZenitIrOperand rvalue;
};

/*
 * Function: zenit_ir_instruction_new
 *  Creates and returns a new instruction based on the provided *type*
 *
 * Parameters:
 *  <enum ZenitIrInstructionType> type - Type of the instruction to be created
 *
 * Returns:
 *  <struct ZenitIrInstruction>* - Instruction object
 *
 * Notes:
 *  The object returned by this function must be freed with the
 *  <zenit_ir_instruction_free> function
 */
struct ZenitIrInstruction* zenit_ir_instruction_new(enum ZenitIrInstructionType type);

/*
 * Function: zenit_ir_instruction_free
 *  Releases the memory used by an instruction object
 *
 * Parameters:
 *  <struct ZenitIrInstruction> *instruction - The instruction object to be freed
 *
 * Returns:
 *  void - This function does not return a value
 *
 */
void zenit_ir_instruction_free(struct ZenitIrInstruction *instruction);

/*
 * Function: zenit_ir_instruction_dump
 *  Dumps the string representation of the instruction to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zenit_ir_instruction_dump(instruction, output);
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
char* zenit_ir_instruction_dump(struct ZenitIrInstruction *instruction, char *output);

#endif /* ZENIT_IR_INSTRUCTION_H */
