#ifndef ZIR_TYPE_ARRAY_H
#define ZIR_TYPE_ARRAY_H

#include <stdlib.h>
#include "type.h"

/*
 * Struct: struct ZirArrayType
 *  Represents an array type
 * 
 * Members:
 *  <struct ZirType> base: Base type information
 *  <struct ZirType> *member_type: The type of the array's members
 *  <size_t> length: The number of elements within the array
 */
struct ZirArrayType {
    struct ZirType base;
    struct ZirType *member_type;
    size_t length;
};

/*
 * Function: zir_type_array_new
 *  Returns a new instance of an array type
 *
 * Parameters:
 *  <struct ZirType> *member_type: A type object that represents the type of the array's members
 *
 * Returns:
 *  struct ZirArrayType*: Pointer to a an array type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_type_array_free> function
 */
struct ZirArrayType* zir_type_array_new(struct ZirType *member_type);

/*
 * Function: zir_type_array_hash
 *  Returns a hash that identifies the array type object
 *
 * Parameters:
 *  <struct ZirArrayType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zir_type_array_hash(struct ZirArrayType *type);

/*
 * Function: zir_type_array_to_string
 *  Returns a string representation of the array type object
 *
 * Parameters:
 *  <struct ZirArrayType> *type: Type object
 *
 * Returns:
 *  char*: String representation of the array type
 *
 * Notes:
 *  The string returned by this function MUST NOT be freed by the caller, the type object
 *  has ownership of it, and it frees the string memory when the function <zir_type_array_free>
 *  is used to free the memory used by the type object.
 */
char* zir_type_array_to_string(struct ZirArrayType *type);

/*
 * Function: zir_type_array_equals
 *  Check if the array type *type_a* is equals to the *type_b* object
 *
 * Parameters:
 *  <struct ZirArrayType> *type_a: Array type to compare 
 *  <struct ZirType> *type_b: Type to compare against the array type
 *
 * Returns:
 *  bool: *true* if *type_b* is an array type equals to *type_a*, otherwise it returns *false*
 */
bool zir_type_array_equals(struct ZirArrayType *type_a, struct ZirType *type_b);

/*
 * Function: zir_type_array_is_assignable_from
 *  Checks if an object with type equals to *value_type* can be assigned to an object with an array type
 *  equals to *target_type*
 *
 * Parameters:
 *  <struct ZirArrayType> *target_type: Target array type for the assignment
 *  <struct ZirType> *value_type: Source type of the assignment
 *
 * Returns:
 *  bool: *true* if an object of type *value_type* can be assigned to an object with type *target_type*,
 *        otherwise this function returns *false*.
 */
bool zir_type_array_is_assignable_from(struct ZirArrayType *target_type, struct ZirType *value_type);

/*
 * Function: zir_type_array_is_castable_to
 *  Checks if an object with an array type equals to *array_type* can be casted to an object of type equals
 *  to *target_type*
 *
 * Parameters:
 *  <struct ZirArrayType> *array_type: Source type of the cast
 *  <struct ZirType> *target_type: Target type of the cast
 *
 * Returns:
 *  bool: *true* if the array type can be casted to a *target_type*, otherwise *false*
 */
bool zir_type_array_is_castable_to(struct ZirArrayType *array_type, struct ZirType *target_type);

/*
 * Function: zir_type_array_size
 *  Returns the size needed to store an instance of the array type (in bytes)
 *
 * Parameters:
 *  <struct ZirArrayType> *type: Array type object
 *
 * Returns:
 *  size_t: Size needed to store an instance of the array type
 */
size_t zir_type_array_size(struct ZirArrayType *type);

/*
 * Function: zir_type_array_free
 *  Frees the memory of the array type object
 *
 * Parameters:
 *  <struct ZirArrayType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_type_array_free(struct ZirArrayType *type);

#endif /* ZIR_TYPE_ARRAY_H */
