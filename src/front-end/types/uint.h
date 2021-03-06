#ifndef ZENIT_TYPE_UINT_H
#define ZENIT_TYPE_UINT_H


#include "type.h"

/*
 * Enum: ZenitUintTypeSize
 *  Represents the set of possible sizes of a uint type
 */
typedef enum ZenitUintTypeSize {
    ZENIT_UINT_UNK,
    ZENIT_UINT_8,
    ZENIT_UINT_16
} ZenitUintTypeSize;

/*
 * Struct: ZenitUintType
 *  Represents a uint type
 * 
 * Members:
 *  <ZenitType> base: Base type information
 *  <ZenitUintTypeSize> size: Size of the uint
 */
typedef struct ZenitUintType {
    ZenitType base;
    ZenitUintTypeSize size;
} ZenitUintType;

/*
 * Function: zenit_uint_type_new
 *  Returns a new instance of a uint type
 *
 * Parameters:
 *  <ZenitUintTypeSize> size: The size of the uint
 *
 * Returns:
 *  ZenitUintType*: Pointer to a a uint type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_uint_type_free> function
 */
ZenitUintType* zenit_uint_type_new(ZenitUintTypeSize size);

/*
 * Function: zenit_uint_type_size_from_slice
 *  Returns the size of a uint encoded in its name that is contained in a slice of bytes
 *  representing a string
 *
 * Parameters:
 *  <struct FlSlice> *slice: Slice of bytes that represents a string that contains the size information
 *
 * Returns:
 *  ZenitUintTypeSize: The size of the uint encoded in the slice
 */
ZenitUintTypeSize zenit_uint_type_size_from_slice(struct FlSlice *slice);

/*
 * Function: zenit_uint_type_hash
 *  Returns a hash that identifies the uint type object
 *
 * Parameters:
 *  <ZenitUintType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zenit_uint_type_hash(ZenitUintType *type);

/*
 * Function: zenit_uint_type_to_string
 *  Returns a string representation of the uint type object
 *
 * Parameters:
 *  <ZenitUintType> *type: Type object
 *
 * Returns:
 *  char*: String representation of the uint type
 *
 * Notes:
 *  The string returned by this function MUST NOT be freed by the caller, the type object
 *  has ownership of it, and it frees the string memory when the function <zenit_uint_type_free>
 *  is used to free the memory used by the type object.
 */
char* zenit_uint_type_to_string(ZenitUintType *type);

/*
 * Function: zenit_uint_type_equals
 *  Check if the uint type *type_a* is equals to the *type_b* object
 *
 * Parameters:
 *  <ZenitUintType> *type_a: Uint type to compare 
 *  <ZenitType> *type_b: Type to compare against the uint type
 *
 * Returns:
 *  bool: *true* if *type_b* is a uint type equals to *type_a*, otherwise it returns *false*
 */
bool zenit_uint_type_equals(ZenitUintType *type_a, ZenitType *type_b);

/*
 * Function: zenit_uint_type_is_assignable_from
 *  Checks if an object with type equals to *value_type* can be assigned to an object with a uint type
 *  equals to *target_type*
 *
 * Parameters:
 *  <ZenitUintType> *target_type: Target uint type for the assignment
 *  <ZenitType> *value_type: Source type of the assignment
 *
 * Returns:
 *  bool: *true* if an object of type *value_type* can be assigned to an object with type *target_type*,
 *        otherwise this function returns *false*.
 */
bool zenit_uint_type_is_assignable_from(ZenitUintType *target_type, ZenitType *value_type);

/*
 * Function: zenit_uint_type_is_castable_to
 *  Checks if an object with a uint type equals to *uint_type* can be casted to an object of type equals
 *  to *target_type*
 *
 * Parameters:
 *  <ZenitUintType> *uint_type: Source type of the cast
 *  <ZenitType> *target_type: Target type of the cast
 *
 * Returns:
 *  bool: *true* if the uint type can be casted to a *target_type*, otherwise *false*
 */
bool zenit_uint_type_is_castable_to(ZenitUintType *uint_type, ZenitType *target_type);

/*
 * Function: zenit_uint_type_can_unify
 *  Checks if the uint type object can be unified with the type represented by *type_b*
 *
 * Parameters:
 *  <ZenitUintType> *uint_type: Uint type object
 *  <ZenitType> *type_b: Type object
 *
 * Returns:
 *  bool: *true* if the types can be unified, which means they can both be represented by an ancestor
 *        or enclosing -base- type, otherwise *false*.
 */
bool zenit_uint_type_can_unify(ZenitUintType *uint_type, ZenitType *type_b);

/*
 * Function: zenit_uint_type_free
 *  Frees the memory of the uint type object
 *
 * Parameters:
 *  <ZenitUintType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_uint_type_free(ZenitUintType *type);

#endif /* ZENIT_TYPE_UINT_H */
