#ifndef ZIR_TYPE_UINT_H
#define ZIR_TYPE_UINT_H


#include <fllib/Slice.h>
#include "type.h"

/*
 * Enum: ZirUintTypeSize
 *  Represents the set of possible sizes of a uint type
 */
typedef enum ZirUintTypeSize {
    ZIR_UINT_UNK,
    ZIR_UINT_8,
    ZIR_UINT_16
} ZirUintTypeSize;

/*
 * Struct: ZirUintType
 *  Represents a uint type
 * 
 * Members:
 *  <ZirType> base: Base type information
 *  <ZirUintTypeSize> size: Size of the uint
 */
typedef struct ZirUintType {
    ZirType base;
    ZirUintTypeSize size;
} ZirUintType;

/*
 * Function: zir_type_uint_new
 *  Returns a new instance of a uint type
 *
 * Parameters:
 *  <ZirUintTypeSize> size: The size of the uint
 *
 * Returns:
 *  ZirUintType*: Pointer to a a uint type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_type_uint_free> function
 */
ZirUintType* zir_type_uint_new(ZirUintTypeSize size);

/*
 * Function: zir_type_uint_hash
 *  Returns a hash that identifies the uint type object
 *
 * Parameters:
 *  <ZirUintType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zir_type_uint_hash(ZirUintType *type);

/*
 * Function: zir_type_uint_to_string
 *  Returns a string representation of the uint type object
 *
 * Parameters:
 *  <ZirUintType> *type: Type object
 *
 * Returns:
 *  char*: String representation of the uint type
 *
 * Notes:
 *  The string returned by this function MUST NOT be freed by the caller, the type object
 *  has ownership of it, and it frees the string memory when the function <zir_type_uint_free>
 *  is used to free the memory used by the type object.
 */
char* zir_type_uint_to_string(ZirUintType *type);

/*
 * Function: zir_type_uint_equals
 *  Check if the uint type *type_a* is equals to the *type_b* object
 *
 * Parameters:
 *  <ZirUintType> *type_a: Uint type to compare 
 *  <ZirType> *type_b: Type to compare against the uint type
 *
 * Returns:
 *  bool: *true* if *type_b* is a uint type equals to *type_a*, otherwise it returns *false*
 */
bool zir_type_uint_equals(ZirUintType *type_a, ZirType *type_b);

/*
 * Function: zir_type_uint_is_assignable_from
 *  Checks if an object with type equals to *value_type* can be assigned to an object with a uint type
 *  equals to *target_type*
 *
 * Parameters:
 *  <ZirUintType> *target_type: Target uint type for the assignment
 *  <ZirType> *value_type: Source type of the assignment
 *
 * Returns:
 *  bool: *true* if an object of type *value_type* can be assigned to an object with type *target_type*,
 *        otherwise this function returns *false*.
 */
bool zir_type_uint_is_assignable_from(ZirUintType *target_type, ZirType *value_type);

/*
 * Function: zir_type_uint_size
 *  Returns the size needed to store an instance of the uint type (in bytes)
 *
 * Parameters:
 *  <ZirUintType> *type: Uint type object
 *
 * Returns:
 *  size_t: Size needed to store an instance of the uint type
 */
size_t zir_type_uint_size(ZirUintType *type);

/*
 * Function: zir_type_uint_is_castable_to
 *  Checks if an object with a uint type equals to *uint_type* can be casted to an object of type equals
 *  to *target_type*
 *
 * Parameters:
 *  <ZirUintType> *uint_type: Source type of the cast
 *  <ZirType> *target_type: Target type of the cast
 *
 * Returns:
 *  bool: *true* if the uint type can be casted to a *target_type*, otherwise *false*
 */
bool zir_type_uint_is_castable_to(ZirUintType *uint_type, ZirType *target_type);

/*
 * Function: zir_type_uint_free
 *  Frees the memory of the uint type object
 *
 * Parameters:
 *  <ZirUintType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_type_uint_free(ZirUintType *type);

#endif /* ZIR_TYPE_UINT_H */
