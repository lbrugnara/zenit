#ifndef ZIR_TYPE_BOOL_H
#define ZIR_TYPE_BOOL_H

#include <fllib/Slice.h>
#include "type.h"

/*
 * Struct: struct ZirBoolType
 *  Represents a boolean type
 * 
 * Members:
 *  <struct ZirType> base: Base type information
 */
struct ZirBoolType {
    struct ZirType base;
};

/*
 * Function: zir_type_bool_new
 *  Returns a new instance of a boolean type
 *
 * Parameters:
 *  This function does not take parameters
 *
 * Returns:
 *  struct ZirBoolType*: Pointer to a a boolean type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_type_bool_free> function
 */
struct ZirBoolType* zir_type_bool_new(void);

/*
 * Function: zir_type_bool_hash
 *  Returns a hash that identifies the boolean type object
 *
 * Parameters:
 *  <struct ZirBoolType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zir_type_bool_hash(struct ZirBoolType *type);

/*
 * Function: zir_type_bool_to_string
 *  Returns a string representation of the boolean type object
 *
 * Parameters:
 *  <struct ZirBoolType> *type: Type object
 *
 * Returns:
 *  char*: String representation of the boolean type
 *
 * Notes:
 *  The string returned by this function MUST NOT be freed by the caller, the type object
 *  has ownership of it, and it frees the string memory when the function <zir_type_bool_free>
 *  is used to free the memory used by the type object.
 */
char* zir_type_bool_to_string(struct ZirBoolType *type);

/*
 * Function: zir_type_bool_equals
 *  Check if the boolean type *type_a* is equals to the *type_b* object
 *
 * Parameters:
 *  <struct ZirBoolType> *type_a: Boolean type to compare 
 *  <struct ZirType> *type_b: Type to compare against the boolean type
 *
 * Returns:
 *  bool: *true* if *type_b* is a boolean type equals to *type_a*, otherwise it returns *false*
 */
bool zir_type_bool_equals(struct ZirBoolType *type_a, struct ZirType *type_b);

/*
 * Function: zir_type_bool_is_assignable_from
 *  Checks if an object with type equals to *value_type* can be assigned to an object with a boolean type
 *  equals to *target_type*
 *
 * Parameters:
 *  <struct ZirBoolType> *target_type: Target boolean type for the assignment
 *  <struct ZirType> *value_type: Source type of the assignment
 *
 * Returns:
 *  bool: *true* if an object of type *value_type* can be assigned to an object with type *target_type*,
 *        otherwise this function returns *false*.
 */
bool zir_type_bool_is_assignable_from(struct ZirBoolType *target_type, struct ZirType *value_type);

/*
 * Function: zir_type_bool_size
 *  Returns the size needed to store an instance of the boolean type (in bytes)
 *
 * Parameters:
 *  <struct ZirBoolType> *type: Boolean type object
 *
 * Returns:
 *  size_t: Size needed to store an instance of the boolean type
 */
size_t zir_type_bool_size(struct ZirBoolType *type);

/*
 * Function: zir_type_bool_is_castable_to
 *  Checks if an object with a boolean type equals to *bool_type* can be casted to an object of type equals
 *  to *target_type*
 *
 * Parameters:
 *  <struct ZirBoolType> *bool_type: Source type of the cast
 *  <struct ZirType> *target_type: Target type of the cast
 *
 * Returns:
 *  bool: *true* if the boolean type can be casted to a *target_type*, otherwise *false*
 */
bool zir_type_bool_is_castable_to(struct ZirBoolType *bool_type, struct ZirType *target_type);

/*
 * Function: zir_type_bool_free
 *  Frees the memory of the boolean type object
 *
 * Parameters:
 *  <struct ZirBoolType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_type_bool_free(struct ZirBoolType *type);

#endif /* ZIR_TYPE_BOOL_H */
