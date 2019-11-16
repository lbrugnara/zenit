#ifndef ZENIT_IR_TYPE_H
#define ZENIT_IR_TYPE_H

#include <stdlib.h>
#include <fllib.h>

/*
 * Enum: enum ZenitIrType
 *  Enumerates all the native type supported by ZIR
 *  including some special values that the compiler
 *  uses internally
 *
 */
enum ZenitIrType {
    ZENIT_IR_TYPE_NONE,
    ZENIT_IR_TYPE_CUSTOM,
    ZENIT_IR_TYPE_UINT8,
    ZENIT_IR_TYPE_UINT16,
};


/*
 * Struct: struct ZenitIrTypeInfo
 *  Represents the information of a type. 
 * 
 * Members:
 *  <const char> *name: If the <enum ZenitIrType> is <ZENIT_IR_TYPE_CUSTOM> this property should be present, otherwise NULL.
 *  <enum ZenitIrType> type: The raw type.
 *  <size_t> elements: Number of elements (1 for simple variables. 0, 1 or greater for arrays)
 *  <bool> is_array: True if the variable is an array
 *  <bool> is_ref: True if the variable is a reference to a variable
 *  
 */
struct ZenitIrTypeInfo {
    const char *name;
    enum ZenitIrType type;
    size_t elements;
    bool is_array;
    bool is_ref;
};

/*
 * Function: zenit_ir_type_to_string
 *  Returns a string representation of the <struct ZenitIrTypeInfo> object.
 *  If the type is an array type, the string representation contains
 *  the size information. If the the type is a reference it also contains that
 *  information.
 *
 * Parameters:
 *  typeinfo - <struct ZenitIrTypeInfo> object to get its string representation.
 *
 * Returns:
 *  const char* - String representation of the *typeinfo* object
 * 
 * Notes:
 *  To get the base type without any other
 *  information refer to the <zenit_ir_type_to_base_string> function
 *
 */
const char* zenit_ir_type_to_string(const struct ZenitIrTypeInfo *typeinfo);

/*
 * Function: zenit_ir_type_to_base_string
 *  Returns the string representation of the <enum ZenitIrType> within the *typeinfo*
 *  object without any information regarding if it is an array or a reference.
 *
 * Parameters:
 *  typeinfo - <struct ZenitIrTypeInfo> object to get its <enum ZenitIrType> string representation.
 *
 * Returns:
 *  const char* - String representation of the <enum ZenitIrType> within the *typeinfo* object
 *
 */
const char* zenit_ir_type_to_base_string(const struct ZenitIrTypeInfo *typeinfo);

/*
 * Function: zenit_ir_type_copy
 *  Copies the information from the *src_type* into the *dest_type*
 *
 * Parameters:
 *  dest_type - Destination type object
 *  src_type - Source type object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_ir_type_copy(struct ZenitIrTypeInfo *dest_type, struct ZenitIrTypeInfo *src_type);

/*
 * Function: zenit_ir_type_equals
 *  Compares *type_a* and *type_b* to know if they are equals, including
 *  if they are arrays and their elements count
 *
 * Parameters:
 *  type_a - Object to compare
 *  type_b - Object to compare
 *
 * Returns:
 *  bool - *true* if the objects are equals, otherwise *false*.
 *
 */
bool zenit_ir_type_equals(struct ZenitIrTypeInfo *type_a, struct ZenitIrTypeInfo *type_b);

#endif /* ZENIT_IR_TYPE_H */
