#ifndef ZIR_OPERAND_H
#define ZIR_OPERAND_H


#include "../../types/type.h"

/*
 * Enum: ZirOperandType
 *  Represents the type of operands that can be part of a <ZirInstr> object
 */
typedef enum ZirOperandType {
    ZIR_OPERAND_UINT,
    ZIR_OPERAND_BOOL,
    ZIR_OPERAND_ARRAY,
    ZIR_OPERAND_STRUCT,
    ZIR_OPERAND_SYMBOL,
    ZIR_OPERAND_REFERENCE
} ZirOperandType;

/*
 * Struct: ZirOperand
 *  Represents an operand of a <ZirInstr>
 * 
 * Members:
 *  <ZirOperandType> type: The type of operand this object represents
 */
typedef struct ZirOperand {
    ZirOperandType type;
} ZirOperand;

/*
 * Function: zir_operand_free
 *  This function frees the memory used by the operand object
 *
 * Parameters:
 *  <ZirOperand> *operand: Operand object from where to free some members based on the <ZirOperandType>
 *
 * Returns:
 *  <void>: This function does not return a value
 */
void zir_operand_free(ZirOperand *operand);

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
 *  <ZirOperand> *operand: Operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  char*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_dump(ZirOperand *operand, char *output);

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
 *  <ZirOperand> *operand: Operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_type_dump(ZirOperand *operand, char *output);

#endif /* ZIR_OPERAND_H */
