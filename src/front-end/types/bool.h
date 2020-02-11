#ifndef ZENIT_TYPE_BOOL_H
#define ZENIT_TYPE_BOOL_H


#include "type.h"

/*
 * Struct: struct ZenitBoolType
 *  Represents a bool type
 * 
 * Members:
 *  <struct ZenitType> base: Base type information
 */
struct ZenitBoolType {
    struct ZenitType base;
};

/*
 * Function: zenit_type_bool_new
 *  Returns a new instance of a bool type
 *
 * Parameters:
 *  This function does not take parameters
 *
 * Returns:
 *  struct ZenitBoolType*: Pointer to a a bool type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_type_bool_free> function
 */
struct ZenitBoolType* zenit_type_bool_new(void);

/*
 * Function: zenit_type_bool_hash
 *  Returns a hash that identifies the bool type object
 *
 * Parameters:
 *  <struct ZenitBoolType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zenit_type_bool_hash(struct ZenitBoolType *type);

/*
 * Function: zenit_type_bool_to_string
 *  Returns a string representation of the bool type object
 *
 * Parameters:
 *  <struct ZenitBoolType> *type: Type object
 *
 * Returns:
 *  char*: String representation of the bool type
 *
 * Notes:
 *  The string returned by this function MUST NOT be freed by the caller, the type object
 *  has ownership of it, and it frees the string memory when the function <zenit_type_bool_free>
 *  is used to free the memory used by the type object.
 */
char* zenit_type_bool_to_string(struct ZenitBoolType *type);

/*
 * Function: zenit_type_bool_equals
 *  Check if the bool type *type_a* is equals to the *type_b* object
 *
 * Parameters:
 *  <struct ZenitBoolType> *type_a: Uint type to compare 
 *  <struct ZenitType> *type_b: Type to compare against the bool type
 *
 * Returns:
 *  bool: *true* if *type_b* is a bool type equals to *type_a*, otherwise it returns *false*
 */
bool zenit_type_bool_equals(struct ZenitBoolType *type_a, struct ZenitType *type_b);

/*
 * Function: zenit_type_bool_is_assignable_from
 *  Checks if an object with type equals to *value_type* can be assigned to an object with a bool type
 *  equals to *target_type*
 *
 * Parameters:
 *  <struct ZenitBoolType> *target_type: Target bool type for the assignment
 *  <struct ZenitType> *value_type: Source type of the assignment
 *
 * Returns:
 *  bool: *true* if an object of type *value_type* can be assigned to an object with type *target_type*,
 *        otherwise this function returns *false*.
 */
bool zenit_type_bool_is_assignable_from(struct ZenitBoolType *target_type, struct ZenitType *value_type);

/*
 * Function: zenit_type_bool_is_castable_to
 *  Checks if an object with a bool type equals to *bool_type* can be casted to an object of type equals
 *  to *target_type*
 *
 * Parameters:
 *  <struct ZenitBoolType> *bool_type: Source type of the cast
 *  <struct ZenitType> *target_type: Target type of the cast
 *
 * Returns:
 *  bool: *true* if the bool type can be casted to a *target_type*, otherwise *false*
 */
bool zenit_type_bool_is_castable_to(struct ZenitBoolType *bool_type, struct ZenitType *target_type);

/*
 * Function: zenit_type_bool_can_unify
 *  Checks if the bool type object can be unified with the type represented by *type_b*
 *
 * Parameters:
 *  <struct ZenitBoolType> *bool_type: Uint type object
 *  <struct ZenitType> *type_b: Type object
 *
 * Returns:
 *  bool: *true* if the types can be unified, which means they can both be represented by an ancestor
 *        or enclosing -base- type, otherwise *false*.
 */
bool zenit_type_bool_can_unify(struct ZenitBoolType *bool_type, struct ZenitType *type_b);

/*
 * Function: zenit_type_bool_free
 *  Frees the memory of the bool type object
 *
 * Parameters:
 *  <struct ZenitBoolType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_type_bool_free(struct ZenitBoolType *type);

#endif /* ZENIT_TYPE_BOOL_H */
