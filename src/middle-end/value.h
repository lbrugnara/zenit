#ifndef ZIR_VALUE_H
#define ZIR_VALUE_H

#include "type.h"
#include "operand.h"

/*
 * Enum: enum ZirValueType
 *  A value in ZIR is a primitive value (literals and arrays by now)
 */
enum ZirValueType {
    ZIR_VALUE_LITERAL,
    ZIR_VALUE_ARRAY
};

/*
 * Struct: struct ZirValue
 *  The base object for all the value objects to be represented in ZIR
 * 
 * Members:
 *  <enum ZirValueType> type: The raw type of the value
 *  <struct ZirTypeInfo> typeinfo: The type information object of the value object
 * 
 */
struct ZirValue {
    enum ZirValueType type;
    struct ZirTypeInfo typeinfo;
};

/*
 * Struct: struct ZirLiteralValue
 *  Represents a literal value in ZIR
 * 
 * Members:
 *  <struct ZirValue> base: Basic information of the object 
 *  <anonymous union> value: Based on the type information the union is populated with a C value that maps to a ZIR native type
 * 
 */
struct ZirLiteralValue {
    struct ZirValue base;
    union {
        uint8_t uint8;
        uint16_t uint16;
    } value;
};

/*
 * Struct: struct ZirArrayValue
 *  Represents a literal array value
 * 
 * Members:
 *  <struct ZirValue> base: Basic information of the node object
 *  <struct ZirOperand> *elements: Array of operands that are part of this value object
 */
struct ZirArrayValue {
    struct ZirValue base;
    struct ZirOperand *elements;
};

/*
 * Function: zir_value_new
 *  Returns a pointer to a value object of the requested type
 *
 * Parameters:
 *  type - Type of the requested value object
 *
 * Returns:
 *  struct ZirValue* - Pointer to the value object
 *
 * Notes:
 *  The memory allocated by this function must be released with
 *  the <zir_value_free> function
 */
struct ZirValue* zir_value_new(enum ZirValueType type);

/*
 * Function: zir_value_free
 *  Frees the memory allocated by a <struct ZirValue> object
 *
 * Parameters:
 *  value - Value object to be freed.
 *
 * Returns:
 *  void - This function does not return a value
 */
void zir_value_free(struct ZirValue *value);

/*
 * Function: zir_value_dump
 *  Dumps the string representation of the value to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_value_dump(value, output);
 * ===========
 *
 * Parameters:
 *  value - Value object
 *  output - Output buffer
 *
 * Returns:
 *  char* - *output* pointer
 *
 */
char* zir_value_dump(struct ZirValue *value, char *output);

#endif /* ZIR_VALUE_H */
