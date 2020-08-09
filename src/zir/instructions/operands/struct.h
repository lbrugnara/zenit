#ifndef ZIR_OPERAND_STRUCT_H
#define ZIR_OPERAND_STRUCT_H

#include "operand.h"
#include "../../types/struct.h"

typedef struct ZirStructOperandMember {
    const char *name;
    ZirOperand *operand;
} ZirStructOperandMember;

/*
 * Struct: ZirStructOperand
 *  A struct operand contains information about the type of its members and references to the 
 *  operands that represents each of those members
 * 
 * Members:
 *  <ZirOperand> base: Basic operand information
 *  <ZirStructType> *type: The type information of the struct members
 *  <ZirOperand> **members: Pointer to the operands for each struct member
 */
typedef struct ZirStructOperand {
    ZirOperand base;
    ZirStructType *type;
    ZirStructOperandMember **members;
} ZirStructOperand;

/*
 * Function: zir_operand_struct_new
 *  Creates a new struct operand object with an empty set of members. The *type* object represents
 *  the type of the struct operand
 *
 * Parameters:
 *  <ZirStructType> *type: The type of the struct operand
 *
 * Returns:
 *  ZirStructOperand*: The new struct operand object
 *
 * Notes:
 *  The object returned by this function must be freed using the <zir_operand_struct_free> function.
 *  The struct operand takes ownership of the <ZirStructType> object, which means it will release
 *  the type object memory too when <zir_operand_struct_free> is called with the struct operand as argument.
 */
ZirStructOperand* zir_operand_struct_new(ZirStructType *type);

/*
 * Function: zir_operand_struct_add_member
 *  Adds the *member_operand* object to the list of members of the struct operand
 *
 * Parameters:
 *  <ZirStructOperand> *struct_operand: The struct operand object
 *  <const char> *name: The struct member name
 *  <ZirOperand> *operand: The operand associated to the struct member
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_operand_struct_add_member(ZirStructOperand *struct_operand, const char *name, ZirOperand *operand);

/*
 * Function: zir_operand_struct_free
 *  Frees the memory of the struct operand
 *
 * Parameters:
 *  <ZirStructOperand> *struct_operand: Struct operand object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_operand_struct_free(ZirStructOperand *struct_operand);

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
 *  <ZirStructOperand> *struct_operand: Struct operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_struct_dump(ZirStructOperand *struct_operand, char *output);

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
 *  <ZirStructOperand> *struct_operand: Operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_struct_type_dump(ZirStructOperand *struct_operand, char *output);

#endif /* ZIR_OPERAND_STRUCT_H */
