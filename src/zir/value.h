#ifndef ZENIT_IR_VALUE_H
#define ZENIT_IR_VALUE_H

#include "type.h"
#include "operand.h"

/*
 * Enum: enum ZenitIrValueType
 *  A value in ZIR is a primitive value (literals and arrays by now)
 */
enum ZenitIrValueType {
    ZENIT_IR_VALUE_LITERAL,
    ZENIT_IR_VALUE_ARRAY
};

/*
 * Struct: struct ZenitIrValue
 *  The base object for all the value objects to be represented in ZIR
 * 
 * Members:
 *  <enum ZenitIrValueType> type: The raw type of the value
 *  <struct ZenitIrTypeInfo> typeinfo: The type information object of the value object
 * 
 */
struct ZenitIrValue {
    enum ZenitIrValueType type;
    struct ZenitIrTypeInfo typeinfo;
};

/*
 * Struct: struct ZenitIrLiteralValue
 *  Represents a literal value in ZIR
 * 
 * Members:
 *  <struct ZenitIrValue> base: Basic information of the object 
 *  <anonymous union> value: Based on the type information the union is populated with a C value that maps to a ZIR native type
 * 
 */
struct ZenitIrLiteralValue {
    struct ZenitIrValue base;
    union {
        uint8_t uint8;
        uint16_t uint16;
    } value;
};

/*
 * Struct: struct ZenitIrArrayValue
 *  Represents a literal array value
 * 
 * Members:
 *  <struct ZenitIrValue> base: Basic information of the node object
 *  <struct ZenitIrOperand> *elements: Array of operands that are part of this value object
 */
struct ZenitIrArrayValue {
    struct ZenitIrValue base;
    struct ZenitIrOperand *elements;
};

/*
 * Function: zenit_ir_value_new
 *  Returns a pointer to a value object of the requested type
 *
 * Parameters:
 *  type - Type of the requested value object
 *
 * Returns:
 *  struct ZenitIrValue* - Pointer to the value object
 *
 * Notes:
 *  The memory allocated by this function must be released with
 *  the <zenit_ir_value_free> function
 */
struct ZenitIrValue* zenit_ir_value_new(enum ZenitIrValueType type);

/*
 * Function: zenit_ir_value_free
 *  Frees the memory allocated by a <struct ZenitIrValue> object
 *
 * Parameters:
 *  value - Value object to be freed.
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_ir_value_free(struct ZenitIrValue *value);

/*
 * Function: zenit_ir_value_dump
 *  Dumps the string representation of the value to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zenit_ir_value_dump(value, output);
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
char* zenit_ir_value_dump(struct ZenitIrValue *value, char *output);

#endif /* ZENIT_IR_VALUE_H */
