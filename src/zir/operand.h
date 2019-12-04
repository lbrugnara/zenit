#ifndef ZIR_OPERAND_H
#define ZIR_OPERAND_H

#include "value.h"
#include "symbol.h"

/*
 * Enum: enum ZirOperandType
 *  The type of operands of the different <struct ZirInstruction>s are represented by this
 *  enum.
 */
enum ZirOperandType {
    ZIR_OPERAND_VALUE,
    ZIR_OPERAND_SYMBOL,
    ZIR_OPERAND_REFERENCE
};

/*
 * Struct: struct ZirOperand
 *  Represents an operand of a <struct ZirInstruction>
 */
struct ZirOperand {
    enum ZirOperandType type;
    union {
        struct ZirValue *value;
        struct ZirSymbol *symbol;
    } operand;
};

/*
 * Function: zir_operand_free
 *  This function frees memory used by some type of operand's members
 *
 * Parameters:
 *  operand - Operand object from where to free some members based on the <enum ZirOperandType>
 *
 * Returns:
 *  void - This function does not return a value
 */
void zir_operand_free(struct ZirOperand *operand);

/*
 * Function: zir_operand_dump
 *  Dumps the string representation of the operand to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_operand_dump(operand, output);
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
char* zir_operand_dump(struct ZirOperand *operand, char *output);

#endif /* ZIR_OPERAND_H */
