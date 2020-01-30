#ifndef ZIR_OPERAND_ARRAY_H
#define ZIR_OPERAND_ARRAY_H

#include "operand.h"
#include "array.h"

/*
 * Struct: struct ZirArrayOperand
 *  An array operand contains information about the type of its members and references to the 
 *  operands that represents each of those members
 * 
 * Members:
 *  <struct ZirOperand> base: Basic operand information
 *  <struct ZirArrayType> *type: The type information of the array's members
 *  <struct ZirOperand> **elements: Pointer to the operands for each array member
 */
struct ZirArrayOperand {
    struct ZirOperand base;
    struct ZirArrayType *type;
    struct ZirOperand **elements;
};

/*
 * Function: zir_operand_array_new
 *  Creates a new array operand object with an empty set of elements. The *type* object represents
 *  the type of each array member
 *
 * Parameters:
 *  <struct ZirArrayType> *type: The type of the array's members
 *
 * Returns:
 *  struct ZirArrayOperand*: The new array operand object
 *
 * Notes:
 *  The object returned by this function must be freed using the <zir_operand_array_free> function.
 *  The array operand takes ownership of the <struct ZirArrayType> object, which means it will release
 *  they type object memory too when <zir_operand_array_free> is called with the array operand as argument.
 */
struct ZirArrayOperand* zir_operand_array_new(struct ZirArrayType *type);

/*
 * Function: zir_operand_array_add_element
 *  Adds the *member_operand* object to the list of members of the array operand
 *
 * Parameters:
 *  <struct ZirArrayOperand> *array_operand: The array operand object
 *  <struct ZirOperand> *operand: The operand to add as an element of the array operand
 *
 * Returns:
 *  void: This function does not return a value
 *
 * Notes:
 *  The array operand takes ownership of the <struct ZirOperand> object if it is not already owned by some other
 *  object. In the case the array operand takes ownership of the operand, when the <zir_operand_array_free> function 
 *  is called with the array operand as argument, the memory of the operand object is realeased too.
 */
void zir_operand_array_add_element(struct ZirArrayOperand *array_operand, struct ZirOperand *operand);

/*
 * Function: zir_operand_array_free
 *  Frees the memory of the array operand including the memory used by the *type* property and the 
 *  memory of all the owned operands in the *elements* property
 *
 * Parameters:
 *  <struct ZirArrayOperand> *array_operand: Array operand object
 *
 * Returns:
 *  void: This function does not return a value
 *
 * Notes:
 *  Mind that this function may not free all the memory used by the operands in the *element* property
 */
void zir_operand_array_free(struct ZirArrayOperand *array_operand);

/*
 * Function: zir_operand_array_dump
 *  Dumps the string representation of the array operand to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_operand_array_dump(array_operand, output);
 * ===========
 *
 * Parameters:
 *  <struct ZirArrayOperand> *array_operand: Array operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_array_dump(struct ZirArrayOperand *array_operand, char *output);

/*
 * Function: zir_operand_array_type_dump
 *  Dumps the string representation of the type of the array operand to the *output* 
 *  pointer. Because the *output* pointer can be modified this function returns 
 *  the same pointer, so it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_operand_array_type_dump(array_operand, output);
 * ===========
 *
 * Parameters:
 *  <struct ZirArrayOperand> *array_operand: Operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_array_type_dump(struct ZirArrayOperand *array_operand, char *output);

#endif /* ZIR_OPERAND_ARRAY_H */
