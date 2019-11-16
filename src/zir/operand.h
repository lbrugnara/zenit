#ifndef ZENIT_IR_OPERAND_H
#define ZENIT_IR_OPERAND_H

#include "value.h"
#include "symbol.h"

/*
 * Enum: enum ZenitIrOperandType
 *  The type of operands of the different <struct ZenitIrInstruction>s are represented by this
 *  enum.
 */
enum ZenitIrOperandType {
    ZENIT_IR_OPERAND_VALUE,
    ZENIT_IR_OPERAND_SYMBOL,
    ZENIT_IR_OPERAND_REF
};

/*
 * Struct: struct ZenitIrOperand
 *  Represents an operand of a <struct ZenitIrInstruction>
 */
struct ZenitIrOperand {
    enum ZenitIrOperandType type;
    union {
        struct ZenitIrValue *value;
        struct ZenitIrSymbol *symbol;
    } operand;
};

/*
 * Function: zenit_ir_operand_free
 *  This function frees memory used by some type of operand's members
 *
 * Parameters:
 *  operand - Operand object from where to free some members based on the <enum ZenitIrOperandType>
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_ir_operand_free(struct ZenitIrOperand *operand);

/*
 * Function: zenit_ir_operand_dump
 *  Dumps the string representation of the operand to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zenit_ir_operand_dump(operand, output);
 * ===========
 *
 * Parameters:
 *  operand - Operand object
 *  output - Output buffer
 *
 * Returns:
 *  char* - *output* pointer
 *
 */
char* zenit_ir_operand_dump(struct ZenitIrOperand *operand, char *output);

#endif /* ZENIT_IR_OPERAND_H */
