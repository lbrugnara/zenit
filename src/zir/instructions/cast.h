#ifndef ZENIT_IR_CAST_H
#define ZENIT_IR_CAST_H

#include "instruction.h"
#include "../operand.h"
#include "../attribute.h"
#include "../type.h"

struct ZenitIrCastInstruction {
    struct ZenitIrInstruction base;
    struct ZenitIrOperand lvalue;
    struct ZenitIrOperand rvalue;
};

/*
 * Function: zenit_ir_instruction_cast_new
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
 *  <zenit_ir_instruction_cast_free> function
 */
struct ZenitIrCastInstruction* zenit_ir_instruction_cast_new(void);

/*
 * Function: zenit_ir_instruction_cast_free
 *  Releases the memory used by an instruction object
 *
 * Parameters:
 *  <struct ZenitIrInstruction> *instruction - The instruction object to be freed
 *
 * Returns:
 *  void - This function does not return a value
 *
 */
void zenit_ir_instruction_cast_free(struct ZenitIrCastInstruction *instruction);

/*
 * Function: zenit_ir_instruction_cast_dump
 *  Dumps the string representation of the instruction to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zenit_ir_instruction_cast_dump(instruction, output);
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
char* zenit_ir_instruction_cast_dump(struct ZenitIrCastInstruction *instruction, char *output);

#endif /* ZENIT_IR_CAST_H */
