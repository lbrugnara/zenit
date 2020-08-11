#ifndef ZIR_OPERAND_ARRAY_H
#define ZIR_OPERAND_ARRAY_H

#include "operand.h"
#include "../../types/array.h"

/*
 * Struct: ZirArrayOperand
 *  An array operand contains information about the type of its members and references to the 
 *  operands that represents each of those members
 * 
 * Members:
 *  <ZirOperand> base: Basic operand information
 *  <ZirArrayType> *type: The type information of the array's members
 *  <ZirOperand> **elements: Pointer to the operands for each array member
 */
typedef struct ZirArrayOperand {
    ZirOperand base;
    ZirArrayType *type;
    ZirOperand **elements;
} ZirArrayOperand;

/*
 * Function: zir_array_operand_new
 *  Creates a new array operand object with an empty set of elements. The *type* object represents
 *  the type of each array member
 *
 * Parameters:
 *  <ZirArrayType> *type: The type of the array's members
 *
 * Returns:
 *  ZirArrayOperand*: The new array operand object
 *
 * Notes:
 *  The object returned by this function must be freed using the <zir_array_operand_free> function.
 *  The array operand takes ownership of the <ZirArrayType> object, which means it will release
 *  the type object memory too when <zir_array_operand_free> is called with the array operand as argument.
 */
ZirArrayOperand* zir_array_operand_new(ZirArrayType *type);

/*
 * Function: zir_array_operand_add_element
 *  Adds the *member_operand* object to the list of members of the array operand
 *
 * Parameters:
 *  <ZirArrayOperand> *array_operand: The array operand object
 *  <ZirOperand> *operand: The operand to add as an element of the array operand
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_array_operand_add_element(ZirArrayOperand *array_operand, ZirOperand *operand);

/*
 * Function: zir_array_operand_free
 *  Frees the memory of the array operand
 *
 * Parameters:
 *  <ZirArrayOperand> *array_operand: Array operand object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_array_operand_free(ZirArrayOperand *array_operand);

/*
 * Function: zir_array_operand_dump
 *  Dumps the string representation of the array operand to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_array_operand_dump(array_operand, output);
 * ===========
 *
 * Parameters:
 *  <ZirArrayOperand> *array_operand: Array operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_array_operand_dump(ZirArrayOperand *array_operand, char *output);

/*
 * Function: zir_array_operand_type_dump
 *  Dumps the string representation of the type of the array operand to the *output* 
 *  pointer. Because the *output* pointer can be modified this function returns 
 *  the same pointer, so it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_array_operand_type_dump(array_operand, output);
 * ===========
 *
 * Parameters:
 *  <ZirArrayOperand> *array_operand: Operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_array_operand_type_dump(ZirArrayOperand *array_operand, char *output);

#endif /* ZIR_OPERAND_ARRAY_H */
