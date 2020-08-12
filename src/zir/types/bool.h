#ifndef ZIR_TYPE_BOOL_H
#define ZIR_TYPE_BOOL_H

#include <fllib/Slice.h>
#include "type.h"

/*
 * Struct: ZirBoolType
 *  Represents a boolean type
 * 
 * Members:
 *  <ZirType> base: Base type information
 */
typedef struct ZirBoolType {
    ZirType base;
} ZirBoolType;

/*
 * Function: zir_bool_type_new
 *  Returns a new instance of a boolean type
 *
 * Parameters:
 *  This function does not take parameters
 *
 * Returns:
 *  ZirBoolType*: Pointer to a a boolean type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_bool_type_free> function
 */
ZirBoolType* zir_bool_type_new(void);

/*
 * Function: zir_bool_type_hash
 *  Returns a hash that identifies the boolean type object
 *
 * Parameters:
 *  <ZirBoolType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zir_bool_type_hash(ZirBoolType *type);

/*
 * Function: zir_bool_type_to_string
 *  Returns a string representation of the boolean type object
 *
 * Parameters:
 *  <ZirBoolType> *type: Type object
 *
 * Returns:
 *  char*: String representation of the boolean type
 *
 * Notes:
 *  The string returned by this function MUST NOT be freed by the caller, the type object
 *  has ownership of it, and it frees the string memory when the function <zir_bool_type_free>
 *  is used to free the memory used by the type object.
 */
char* zir_bool_type_to_string(ZirBoolType *type);

/*
 * Function: zir_bool_type_equals
 *  Check if the boolean type *type_a* is equals to the *type_b* object
 *
 * Parameters:
 *  <ZirBoolType> *type_a: Boolean type to compare 
 *  <ZirType> *type_b: Type to compare against the boolean type
 *
 * Returns:
 *  bool: *true* if *type_b* is a boolean type equals to *type_a*, otherwise it returns *false*
 */
bool zir_bool_type_equals(ZirBoolType *type_a, ZirType *type_b);

/*
 * Function: zir_bool_type_is_assignable_from
 *  Checks if an object with type equals to *value_type* can be assigned to an object with a boolean type
 *  equals to *target_type*
 *
 * Parameters:
 *  <ZirBoolType> *target_type: Target boolean type for the assignment
 *  <ZirType> *value_type: Source type of the assignment
 *
 * Returns:
 *  bool: *true* if an object of type *value_type* can be assigned to an object with type *target_type*,
 *        otherwise this function returns *false*.
 */
bool zir_bool_type_is_assignable_from(ZirBoolType *target_type, ZirType *value_type);

/*
 * Function: zir_bool_type_size
 *  Returns the size needed to store an instance of the boolean type (in bytes)
 *
 * Parameters:
 *  <ZirBoolType> *type: Boolean type object
 *
 * Returns:
 *  size_t: Size needed to store an instance of the boolean type
 */
size_t zir_bool_type_size(ZirBoolType *type, size_t ref_size);

/*
 * Function: zir_bool_type_is_castable_to
 *  Checks if an object with a boolean type equals to *bool_type* can be casted to an object of type equals
 *  to *target_type*
 *
 * Parameters:
 *  <ZirBoolType> *bool_type: Source type of the cast
 *  <ZirType> *target_type: Target type of the cast
 *
 * Returns:
 *  bool: *true* if the boolean type can be casted to a *target_type*, otherwise *false*
 */
bool zir_bool_type_is_castable_to(ZirBoolType *bool_type, ZirType *target_type);

/*
 * Function: zir_bool_type_free
 *  Frees the memory of the boolean type object
 *
 * Parameters:
 *  <ZirBoolType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_bool_type_free(ZirBoolType *type);

#endif /* ZIR_TYPE_BOOL_H */
