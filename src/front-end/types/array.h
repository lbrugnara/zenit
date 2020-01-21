#ifndef ZENIT_TYPE_ARRAY_H
#define ZENIT_TYPE_ARRAY_H

#include <stdlib.h>
#include "type.h"

/*
 * Struct: struct ZenitArrayType
 *  Represents an array type
 * 
 * Members:
 *  <struct ZenitType> base: Base type information
 *  <struct ZenitType> *member_type: The type of the array's members
 *  <size_t> length: The number of elements within the array
 */
struct ZenitArrayType {
    struct ZenitType base;
    struct ZenitType *member_type;
    size_t length;
};

/*
 * Function: zenit_type_array_new
 *  Returns a new instance of an array type
 *
 * Parameters:
 *  <struct ZenitType> *member_type: A type object that represents the type of the array's members
 *
 * Returns:
 *  struct ZenitArrayType*: Pointer to a an array type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_type_array_free> function
 */
struct ZenitArrayType* zenit_type_array_new(struct ZenitType *member_type);

/*
 * Function: zenit_type_array_hash
 *  Returns a hash that identifies the array type object
 *
 * Parameters:
 *  <struct ZenitArrayType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zenit_type_array_hash(struct ZenitArrayType *type);

/*
 * Function: zenit_type_array_to_string
 *  Returns a string representation of the array type object
 *
 * Parameters:
 *  <struct ZenitArrayType> *type: Type object
 *
 * Returns:
 *  char*: String representation of the array type
 *
 * Notes:
 *  The string returned by this function MUST NOT be freed by the caller, the type object
 *  has ownership of it, and it frees the string memory when the function <zenit_type_array_free>
 *  is used to free the memory used by the type object.
 */
char* zenit_type_array_to_string(struct ZenitArrayType *type);

/*
 * Function: zenit_type_array_equals
 *  Check if the array type *type_a* is equals to the *type_b* object
 *
 * Parameters:
 *  <struct ZenitArrayType> *type_a: Array type to compare 
 *  <struct ZenitType> *type_b: Type to compare against the array type
 *
 * Returns:
 *  bool: *true* if *type_b* is an array type equals to *type_a*, otherwise it returns *false*
 */
bool zenit_type_array_equals(struct ZenitArrayType *type_a, struct ZenitType *type_b);

/*
 * Function: zenit_type_array_is_assignable_from
 *  Checks if an object with type equals to *value_type* can be assigned to an object with an array type
 *  equals to *target_type*
 *
 * Parameters:
 *  <struct ZenitArrayType> *target_type: Target array type for the assignment
 *  <struct ZenitType> *value_type: Source type of the assignment
 *
 * Returns:
 *  bool: *true* if an object of type *value_type* can be assigned to an object with type *target_type*,
 *        otherwise this function returns *false*.
 */
bool zenit_type_array_is_assignable_from(struct ZenitArrayType *target_type, struct ZenitType *value_type);

/*
 * Function: zenit_type_array_is_castable_to
 *  Checks if an object with an array type equals to *array_type* can be casted to an object of type equals
 *  to *target_type*
 *
 * Parameters:
 *  <struct ZenitArrayType> *array_type: Source type of the cast
 *  <struct ZenitType> *target_type: Target type of the cast
 *
 * Returns:
 *  bool: *true* if the array type can be casted to a *target_type*, otherwise *false*
 */
bool zenit_type_array_is_castable_to(struct ZenitArrayType *array_type, struct ZenitType *target_type);

/*
 * Function: zenit_type_array_can_unify
 *  Checks if the array type object can be unified with the type represented by *type_b*
 *
 * Parameters:
 *  <struct ZenitArrayType> *array_type: Array type object
 *  <struct ZenitType> *type_b: Type object
 *
 * Returns:
 *  bool: *true* if the types can be unified, which means they can both be represented by an ancestor
 *        or enclosing -base- type, otherwise *false*.
 */
bool zenit_type_array_can_unify(struct ZenitArrayType *array_type, struct ZenitType *type_b);

/*
 * Function: zenit_type_array_free
 *  Frees the memory of the array type object
 *
 * Parameters:
 *  <struct ZenitArrayType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_type_array_free(struct ZenitArrayType *type);

#endif /* ZENIT_TYPE_ARRAY_H */
