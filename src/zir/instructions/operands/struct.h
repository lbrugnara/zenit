#ifndef ZIR_OPERAND_STRUCT_H
#define ZIR_OPERAND_STRUCT_H

#include "operand.h"
#include "struct.h"

struct ZirStructOperandMember {
    const char *name;
    struct ZirOperand *operand;
};

/*
 * Struct: struct ZirStructOperand
 *  A struct operand contains information about the type of its members and references to the 
 *  operands that represents each of those members
 * 
 * Members:
 *  <struct ZirOperand> base: Basic operand information
 *  <struct ZirStructType> *type: The type information of the struct members
 *  <struct ZirOperand> **members: Pointer to the operands for each struct member
 */
struct ZirStructOperand {
    struct ZirOperand base;
    struct ZirStructType *type;
    struct ZirStructOperandMember **members;
};

/*
 * Function: zir_operand_struct_new
 *  Creates a new struct operand object with an empty set of members. The *type* object represents
 *  the type of the struct operand
 *
 * Parameters:
 *  <struct ZirStructType> *type: The type of the struct operand
 *
 * Returns:
 *  struct ZirStructOperand*: The new struct operand object
 *
 * Notes:
 *  The object returned by this function must be freed using the <zir_operand_struct_free> function.
 *  The struct operand takes ownership of the <struct ZirStructType> object, which means it will release
 *  the type object memory too when <zir_operand_struct_free> is called with the struct operand as argument.
 */
struct ZirStructOperand* zir_operand_struct_new(struct ZirStructType *type);

/*
 * Function: zir_operand_struct_add_member
 *  Adds the *member_operand* object to the list of members of the struct operand
 *
 * Parameters:
 *  <struct ZirStructOperand> *struct_operand: The struct operand object
 *  <const char> *name: The struct member name
 *  <struct ZirOperand> *operand: The operand associated to the struct member
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_operand_struct_add_member(struct ZirStructOperand *struct_operand, const char *name, struct ZirOperand *operand);

/*
 * Function: zir_operand_struct_free
 *  Frees the memory of the struct operand
 *
 * Parameters:
 *  <struct ZirStructOperand> *struct_operand: Struct operand object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_operand_struct_free(struct ZirStructOperand *struct_operand);

/*
 * Function: zir_operand_struct_dump
 *  Dumps the string representation of the struct operand to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_operand_struct_dump(struct_operand, output);
 * ===========
 *
 * Parameters:
 *  <struct ZirStructOperand> *struct_operand: Struct operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_struct_dump(struct ZirStructOperand *struct_operand, char *output);

/*
 * Function: zir_operand_struct_type_dump
 *  Dumps the string representation of the type of the struct operand to the *output* 
 *  pointer. Because the *output* pointer can be modified this function returns 
 *  the same pointer, so it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_operand_struct_type_dump(struct_operand, output);
 * ===========
 *
 * Parameters:
 *  <struct ZirStructOperand> *struct_operand: Operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_struct_type_dump(struct ZirStructOperand *struct_operand, char *output);

#endif /* ZIR_OPERAND_STRUCT_H */
