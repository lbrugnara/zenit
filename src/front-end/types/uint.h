#ifndef ZENIT_TYPE_UINT_H
#define ZENIT_TYPE_UINT_H

#include <fllib.h>
#include "type.h"

/*
 * Enum: enum ZenitUintTypeSize
 *  Represents the set of possible sizes of a uint type
 */
enum ZenitUintTypeSize {
    ZENIT_UINT_UNK,
    ZENIT_UINT_8,
    ZENIT_UINT_16
};

/*
 * Struct: struct ZenitUintType
 *  Represents a uint type
 * 
 * Members:
 *  <struct ZenitType> base: Base type information
 *  <enum ZenitUintTypeSize> size: Size of the uint
 */
struct ZenitUintType {
    struct ZenitType base;
    enum ZenitUintTypeSize size;
};

/*
 * Function: zenit_type_uint_new
 *  Returns a new instance of a uint type
 *
 * Parameters:
 *  <enum ZenitUintTypeSize> size: The size of the uint
 *
 * Returns:
 *  struct ZenitUintType*: Pointer to a a uint type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_type_uint_free> function
 */
struct ZenitUintType* zenit_type_uint_new(enum ZenitUintTypeSize size);

/*
 * Function: zenit_type_uint_size_from_slice
 *  Returns the size of a uint encoded in its name that is contained in a slice of bytes
 *  representing a string
 *
 * Parameters:
 *  <struct FlSlice> *slice: Slice of bytes that represents a string that contains the size information
 *
 * Returns:
 *  enum ZenitUintTypeSize: The size of the uint encoded in the slice
 */
enum ZenitUintTypeSize zenit_type_uint_size_from_slice(struct FlSlice *slice);

/*
 * Function: zenit_type_uint_hash
 *  Returns a hash that identifies the uint type object
 *
 * Parameters:
 *  <struct ZenitUintType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zenit_type_uint_hash(struct ZenitUintType *type);

/*
 * Function: zenit_type_uint_to_string
 *  Returns a string representation of the uint type object
 *
 * Parameters:
 *  <struct ZenitUintType> *type: Type object
 *
 * Returns:
 *  char*: String representation of the uint type
 *
 * Notes:
 *  The string returned by this function MUST NOT be freed by the caller, the type object
 *  has ownership of it, and it frees the string memory when the function <zenit_type_uint_free>
 *  is used to free the memory used by the type object.
 */
char* zenit_type_uint_to_string(struct ZenitUintType *type);

/*
 * Function: zenit_type_uint_equals
 *  Check if the uint type *type_a* is equals to the *type_b* object
 *
 * Parameters:
 *  <struct ZenitUintType> *type_a: Uint type to compare 
 *  <struct ZenitType> *type_b: Type to compare against the uint type
 *
 * Returns:
 *  bool: *true* if *type_b* is a uint type equals to *type_a*, otherwise it returns *false*
 */
bool zenit_type_uint_equals(struct ZenitUintType *type_a, struct ZenitType *type_b);

/*
 * Function: zenit_type_uint_is_assignable_from
 *  Checks if an object with type equals to *value_type* can be assigned to an object with a uint type
 *  equals to *target_type*
 *
 * Parameters:
 *  <struct ZenitUintType> *target_type: Target uint type for the assignment
 *  <struct ZenitType> *value_type: Source type of the assignment
 *
 * Returns:
 *  bool: *true* if an object of type *value_type* can be assigned to an object with type *target_type*,
 *        otherwise this function returns *false*.
 */
bool zenit_type_uint_is_assignable_from(struct ZenitUintType *target_type, struct ZenitType *value_type);

/*
 * Function: zenit_type_uint_is_castable_to
 *  Checks if an object with a uint type equals to *uint_type* can be casted to an object of type equals
 *  to *target_type*
 *
 * Parameters:
 *  <struct ZenitUintType> *uint_type: Source type of the cast
 *  <struct ZenitType> *target_type: Target type of the cast
 *
 * Returns:
 *  bool: *true* if the uint type can be casted to a *target_type*, otherwise *false*
 */
bool zenit_type_uint_is_castable_to(struct ZenitUintType *uint_type, struct ZenitType *target_type);

/*
 * Function: zenit_type_uint_can_unify
 *  Checks if the uint type object can be unified with the type represented by *type_b*
 *
 * Parameters:
 *  <struct ZenitUintType> *uint_type: Uint type object
 *  <struct ZenitType> *type_b: Type object
 *
 * Returns:
 *  bool: *true* if the types can be unified, which means they can both be represented by an ancestor
 *        or enclosing -base- type, otherwise *false*.
 */
bool zenit_type_uint_can_unify(struct ZenitUintType *uint_type, struct ZenitType *type_b);

/*
 * Function: zenit_type_uint_free
 *  Frees the memory of the uint type object
 *
 * Parameters:
 *  <struct ZenitUintType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_type_uint_free(struct ZenitUintType *type);

#endif /* ZENIT_TYPE_UINT_H */
