#ifndef ZIR_OPERAND_UINT_H
#define ZIR_OPERAND_UINT_H

#include <stdint.h>
#include "operand.h"
#include "../../types/uint.h"

/*
 * Union: ZirUintValue
 *  Holds the value of a ZIR uint operand
 * 
 * Members:
 *  <uint8_t> uint8: Holds a 1 byte uint
 *  <uint16_t> uint16: Holds a 2 bytes uint
 */
typedef union ZirUintValue {
    uint8_t uint8;
    uint16_t uint16;
} ZirUintValue;

/*
 * Struct: ZirUintOperand
 *  A uint operand contains information about the type, size, and value of a uint object
 * 
 * Members:
 *  <ZirOperand> base: Basic operand information
 *  <ZirUintType> *type: The type and size of the uint object
 *  <struct ZirUintValue> value: The value of the uint object
 */
typedef struct ZirUintOperand {
    ZirOperand base;
    ZirUintType *type;
    ZirUintValue value;
} ZirUintOperand;

/*
 * Function: zir_operand_uint_new
 *  Creates a new uint operand with the provided type, size, and value
 *
 * Parameters:
 *  <ZirUintType> *type: The type and size of the uint object
 *  <struct ZirUintValue> value: The value of the uint object
 *
 * Returns:
 *  ZirUintOperand*: The new uint operand object
 *
 * Notes:
 *  The object returned by this function must be freed using the <zir_operand_uint_free> function.
 *  The uint operand takes ownership of the <ZirUintType> object, which means it will release
 *  the type object memory too when <zir_operand_uint_free> is called with the uint operand as argument.
 */
ZirUintOperand* zir_operand_uint_new(ZirUintType *type, ZirUintValue value);

/*
 * Function: zir_operand_uint_free
 *  Frees the memory of the uint operand
 *
 * Parameters:
 *  <ZirUintOperand> *uint_operand: Uint operand object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_operand_uint_free(ZirUintOperand *uint_operand);

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
 *  <ZirUintOperand> *uint_operand: Uint operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_uint_dump(ZirUintOperand *uint_operand, char *output);

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
 *  <ZirUintOperand> *uint_operand: Operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_uint_type_dump(ZirUintOperand *uint_operand, char *output);

#endif /* ZIR_OPERAND_UINT_H */
