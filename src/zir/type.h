#ifndef ZIR_TYPE_H
#define ZIR_TYPE_H

#include <stdlib.h>
#include <fllib.h>

/*
 * Enum: enum ZirType
 *  Enumerates all the native type supported by ZIR
 *  including some special values that the compiler
 *  uses internally
 *
 */
enum ZirType {
    ZIR_TYPE_NONE,
    ZIR_TYPE_CUSTOM,

    ZIR_TYPE_UINT8,
    ZIR_TYPE_UINT16,

    ZIR_TYPE_END,
};


/*
 * Struct: struct ZirTypeInfo
 *  Represents the information of a type. 
 * 
 * Members:
 *  <const char> *name: If the <enum ZirType> is <ZIR_TYPE_CUSTOM> this property should be present, otherwise NULL.
 *  <enum ZirType> type: The raw type.
 *  <size_t> elements: Number of elements (1 for simple variables. 0, 1 or greater for arrays)
 *  <bool> is_array: True if the variable is an array
 *  <bool> is_ref: True if the variable is a reference to a variable
 *  
 */
struct ZirTypeInfo {
    const char *name;
    enum ZirType type;
    size_t elements;
    bool is_array;
    bool is_ref;
};

/*
 * Function: zir_type_to_string
 *  Returns a string representation of the <struct ZirTypeInfo> object.
 *  If the type is an array type, the string representation contains
 *  the size information. If the the type is a reference it also contains that
 *  information.
 *
 * Parameters:
 *  typeinfo - <struct ZirTypeInfo> object to get its string representation.
 *
 * Returns:
 *  const char* - String representation of the *typeinfo* object
 * 
 * Notes:
 *  To get the base type without any other
 *  information refer to the <zir_type_to_base_string> function
 *
 */
const char* zir_type_to_string(const struct ZirTypeInfo *typeinfo);

/*
 * Function: zir_type_to_base_string
 *  Returns the string representation of the <enum ZirType> within the *typeinfo*
 *  object without any information regarding if it is an array or a reference.
 *
 * Parameters:
 *  typeinfo - <struct ZirTypeInfo> object to get its <enum ZirType> string representation.
 *
 * Returns:
 *  const char* - String representation of the <enum ZirType> within the *typeinfo* object
 *
 */
const char* zir_type_to_base_string(const struct ZirTypeInfo *typeinfo);

/*
 * Function: zir_type_copy
 *  Copies the information from the *src_type* into the *dest_type*
 *
 * Parameters:
 *  dest_type - Destination type object
 *  src_type - Source type object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zir_type_copy(struct ZirTypeInfo *dest_type, struct ZirTypeInfo *src_type);

/*
 * Function: zir_type_equals
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
bool zir_type_equals(struct ZirTypeInfo *type_a, struct ZirTypeInfo *type_b);

size_t zir_type_size(struct ZirTypeInfo *type);

#endif /* ZIR_TYPE_H */
