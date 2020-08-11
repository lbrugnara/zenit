#ifndef ZENIT_TYPE_ARRAY_H
#define ZENIT_TYPE_ARRAY_H

#include <stdlib.h>
#include "type.h"

/*
 * Struct: ZenitArrayType
 *  Represents an array type
 * 
 * Members:
 *  <ZenitType> base: Base type information
 *  <ZenitType> *member_type: The type of the array's members
 *  <size_t> length: The number of elements within the array
 */
typedef struct ZenitArrayType {
    ZenitType base;
    ZenitType *member_type;
    size_t length;
} ZenitArrayType;

/*
 * Function: zenit_array_type_new
 *  Returns a new instance of an array type
 *
 * Parameters:
 *  <ZenitType> *member_type: A type object that represents the type of the array's members
 *
 * Returns:
 *  ZenitArrayType*: Pointer to a an array type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_array_type_free> function
 */
ZenitArrayType* zenit_array_type_new(ZenitType *member_type);

/*
 * Function: zenit_array_type_hash
 *  Returns a hash that identifies the array type object
 *
 * Parameters:
 *  <ZenitArrayType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zenit_array_type_hash(ZenitArrayType *type);

/*
 * Function: zenit_array_type_to_string
 *  Returns a string representation of the array type object
 *
 * Parameters:
 *  <ZenitArrayType> *type: Type object
 *
 * Returns:
 *  char*: String representation of the array type
 *
 * Notes:
 *  The string returned by this function MUST NOT be freed by the caller, the type object
 *  has ownership of it, and it frees the string memory when the function <zenit_array_type_free>
 *  is used to free the memory used by the type object.
 */
char* zenit_array_type_to_string(ZenitArrayType *type);

/*
 * Function: zenit_array_type_equals
 *  Check if the array type *type_a* is equals to the *type_b* object
 *
 * Parameters:
 *  <ZenitArrayType> *type_a: Array type to compare 
 *  <ZenitType> *type_b: Type to compare against the array type
 *
 * Returns:
 *  bool: *true* if *type_b* is an array type equals to *type_a*, otherwise it returns *false*
 */
bool zenit_array_type_equals(ZenitArrayType *type_a, ZenitType *type_b);

/*
 * Function: zenit_array_type_is_assignable_from
 *  Checks if an object with type equals to *value_type* can be assigned to an object with an array type
 *  equals to *target_type*
 *
 * Parameters:
 *  <ZenitArrayType> *target_type: Target array type for the assignment
 *  <ZenitType> *value_type: Source type of the assignment
 *
 * Returns:
 *  bool: *true* if an object of type *value_type* can be assigned to an object with type *target_type*,
 *        otherwise this function returns *false*.
 */
bool zenit_array_type_is_assignable_from(ZenitArrayType *target_type, ZenitType *value_type);

/*
 * Function: zenit_array_type_is_castable_to
 *  Checks if an object with an array type equals to *array_type* can be casted to an object of type equals
 *  to *target_type*
 *
 * Parameters:
 *  <ZenitArrayType> *array_type: Source type of the cast
 *  <ZenitType> *target_type: Target type of the cast
 *
 * Returns:
 *  bool: *true* if the array type can be casted to a *target_type*, otherwise *false*
 */
bool zenit_array_type_is_castable_to(ZenitArrayType *array_type, ZenitType *target_type);

/*
 * Function: zenit_array_type_can_unify
 *  Checks if the array type object can be unified with the type represented by *type_b*
 *
 * Parameters:
 *  <ZenitArrayType> *array_type: Array type object
 *  <ZenitType> *type_b: Type object
 *
 * Returns:
 *  bool: *true* if the types can be unified, which means they can both be represented by an ancestor
 *        or enclosing -base- type, otherwise *false*.
 */
bool zenit_array_type_can_unify(ZenitArrayType *array_type, ZenitType *type_b);

/*
 * Function: zenit_array_type_free
 *  Frees the memory of the array type object
 *
 * Parameters:
 *  <ZenitArrayType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_array_type_free(ZenitArrayType *type);

#endif /* ZENIT_TYPE_ARRAY_H */
