#ifndef ZIR_OPERAND_H
#define ZIR_OPERAND_H

#include <fllib.h>
#include "../../types/type.h"

/*
 * Enum: enum ZirOperandType
 *  Represents the type of operands that can be part of a <struct ZirInstruction> object
 */
enum ZirOperandType {
    ZIR_OPERAND_UINT,
    ZIR_OPERAND_ARRAY,
    ZIR_OPERAND_SYMBOL,
    ZIR_OPERAND_REFERENCE
};

/*
 * Struct: struct ZirOperand
 *  Represents an operand of a <struct ZirInstruction>
 * 
 * Members:
 *  <enum ZirOperandType> type: The type of operand this object represents
 *  <void> *owner: The object owner of the operand that is in charge of freeing its memory
 * 
 * Notes:
 *  An operand object can be held by another operand (like arrays or references operands) or by instructions. These
 *  objects are the owner of the operands that are part of them (if these operands don't have a previous owner yet) and
 *  because of that they are in charge of check if it is up to them to call <zir_operand_free> on those operands.
 */
struct ZirOperand {
    enum ZirOperandType type;
};

/*
 * Function: zir_operand_free
 *  This function frees the memory used by the operand object
 *
 * Parameters:
 *  <struct ZirOperand> *operand: Operand object from where to free some members based on the <enum ZirOperandType>
 *
 * Returns:
 *  <void>: This function does not return a value
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
 *  <struct ZirOperand> *operand: Operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  char*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_dump(struct ZirOperand *operand, char *output);

/*
 * Function: zir_operand_type_dump
 *  Dumps the string representation of the type of the operand to the *output* 
 *  pointer. Because the *output* pointer can be modified this function returns 
 *  the same pointer, so it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_operand_type_dump(operand, output);
 * ===========
 *
 * Parameters:
 *  <struct ZirOperand> *operand: Operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_type_dump(struct ZirOperand *operand, char *output);

#endif /* ZIR_OPERAND_H */
