#ifndef ZIR_OPERAND_UINT_H
#define ZIR_OPERAND_UINT_H

#include <stdint.h>
#include "operand.h"

/*
 * Union: union ZirUintValue
 *  Holds the value of a ZIR uint operand
 * 
 * Members:
 *  <uint8_t> uint8: Holds a 1 byte uint
 *  <uint16_t> uint16: Holds a 2 bytes uint
 */
union ZirUintValue {
    uint8_t uint8;
    uint16_t uint16;
};

/*
 * Struct: struct ZirUintOperand
 *  A uint operand contains information about the type, size, and value of a uint object
 * 
 * Members:
 *  <struct ZirOperand> base: Basic operand information
 *  <struct ZirUintType> *type: The type and size of the uint object
 *  <struct ZirUintValue> value: The value of the uint object
 */
struct ZirUintOperand {
    struct ZirOperand base;
    struct ZirUintType *type;
    union ZirUintValue value;
};

/*
 * Function: zir_operand_uint_new
 *  Creates a new uint operand with the provided type, size, and value
 *
 * Parameters:
 *  <struct ZirUintType> *type: The type and size of the uint object
 *  <struct ZirUintValue> value: The value of the uint object
 *
 * Returns:
 *  struct ZirUintOperand*: The new uint operand object
 *
 * Notes:
 *  The object returned by this function must be freed using the <zir_operand_uint_free> function.
 *  The uint operand takes ownership of the <struct ZirUintType> object, which means it will release
 *  the type object memory too when <zir_operand_uint_free> is called with the uint operand as argument.
 */
struct ZirUintOperand* zir_operand_uint_new(struct ZirUintType *type, union ZirUintValue value);

/*
 * Function: zir_operand_uint_free
 *  Frees the memory of the uint operand
 *
 * Parameters:
 *  <struct ZirUintOperand> *uint_operand: Uint operand object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_operand_uint_free(struct ZirUintOperand *uint_operand);

/*
 * Function: zir_operand_uint_dump
 *  Dumps the string representation of the uint operand to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_operand_uint_dump(uint_operand, output);
 * ===========
 *
 * Parameters:
 *  <struct ZirUintOperand> *uint_operand: Uint operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_uint_dump(struct ZirUintOperand *uint_operand, char *output);

/*
 * Function: zir_operand_uint_type_dump
 *  Dumps the string representation of the type of the uint operand to the *output* 
 *  pointer. Because the *output* pointer can be modified this function returns 
 *  the same pointer, so it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_operand_uint_type_dump(uint_operand, output);
 * ===========
 *
 * Parameters:
 *  <struct ZirUintOperand> *uint_operand: Operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_uint_type_dump(struct ZirUintOperand *uint_operand, char *output);

#endif /* ZIR_OPERAND_UINT_H */
