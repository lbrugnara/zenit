#ifndef ZIR_OPERAND_BOOL_H
#define ZIR_OPERAND_BOOL_H

#include <stdint.h>
#include "operand.h"
#include "../../types/bool.h"

/*
 * Struct: ZirBoolOperand
 *  A boolean operand contains information about the type, and value of a boolean object
 * 
 * Members:
 *  <ZirOperand> base: Basic operand information
 *  <ZirBoolType> *type: The type and size of the boolean object
 *  <bool> value: The value of the boolean object
 */
typedef struct ZirBoolOperand {
    ZirOperand base;
    ZirBoolType *type;
    bool value;
} ZirBoolOperand;

/*
 * Function: zir_bool_operand_new
 *  Creates a new boolean operand with the provided type, size, and value
 *
 * Parameters:
 *  <ZirBoolType> *type: The type of the boolean object
 *  <bool> value: The value of the boolean object
 *
 * Returns:
 *  ZirBoolOperand*: The new boolean operand object
 *
 * Notes:
 *  The object returned by this function must be freed using the <zir_bool_operand_free> function.
 *  The boolean operand takes ownership of the <ZirBoolType> object, which means it will release
 *  the type object memory too when <zir_bool_operand_free> is called with the boolean operand as argument.
 */
ZirBoolOperand* zir_bool_operand_new(ZirBoolType *type, bool value);

/*
 * Function: zir_bool_operand_free
 *  Frees the memory of the boolean operand
 *
 * Parameters:
 *  <ZirBoolOperand> *bool_operand: Bool operand object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_bool_operand_free(ZirBoolOperand *bool_operand);

/*
 * Function: zir_bool_operand_dump
 *  Dumps the string representation of the boolean operand to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_bool_operand_dump(bool_operand, output);
 * ===========
 *
 * Parameters:
 *  <ZirBoolOperand> *bool_operand: Bool operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_bool_operand_dump(ZirBoolOperand *bool_operand, char *output);

/*
 * Function: zir_bool_operand_type_dump
 *  Dumps the string representation of the type of the boolean operand to the *output* 
 *  pointer. Because the *output* pointer can be modified this function returns 
 *  the same pointer, so it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_bool_operand_type_dump(bool_operand, output);
 * ===========
 *
 * Parameters:
 *  <ZirBoolOperand> *bool_operand: Operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_bool_operand_type_dump(ZirBoolOperand *bool_operand, char *output);

#endif /* ZIR_OPERAND_BOOL_H */
