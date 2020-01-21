#ifndef ZENIT_TYPE_REFERENCE_H
#define ZENIT_TYPE_REFERENCE_H

#include "type.h"

/*
 * Struct: struct ZenitReferenceType
 *  Represents a reference type
 * 
 * Members:
 *  <struct ZenitType> base: Base type information
 *  <struct ZenitType> *element: The type of the referenced symbol
 */
struct ZenitReferenceType {
    struct ZenitType base;
    struct ZenitType *element;
};

/*
 * Function: zenit_type_reference_new
 *  Returns a new instance of a reference type
 *
 * Parameters:
 *  <struct ZenitType> *element: A type object that represents the type of the referenced symbol
 *
 * Returns:
 *  struct ZenitReferenceType*: Pointer to a a reference type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_type_reference_free> function
 */
struct ZenitReferenceType* zenit_type_reference_new(struct ZenitType *element);

/*
 * Function: zenit_type_reference_hash
 *  Returns a hash that identifies the reference type object
 *
 * Parameters:
 *  <struct ZenitReferenceType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zenit_type_reference_hash(struct ZenitReferenceType *type);

/*
 * Function: zenit_type_reference_to_string
 *  Returns a string representation of the reference type object
 *
 * Parameters:
 *  <struct ZenitReferenceType> *type: Type object
 *
 * Returns:
 *  char*: String representation of the reference type
 *
 * Notes:
 *  The string returned by this function MUST NOT be freed by the caller, the type object
 *  has ownership of it, and it frees the string memory when the function <zenit_type_reference_free>
 *  is used to free the memory used by the type object.
 */
char* zenit_type_reference_to_string(struct ZenitReferenceType *type);

/*
 * Function: zenit_type_reference_equals
 *  Check if the reference type *type_a* is equals to the *type_b* object
 *
 * Parameters:
 *  <struct ZenitReferenceType> *type_a: Reference type to compare 
 *  <struct ZenitType> *type_b: Type to compare against the reference type
 *
 * Returns:
 *  bool: *true* if *type_b* is a reference type equals to *type_a*, otherwise it returns *false*
 */
bool zenit_type_reference_equals(struct ZenitReferenceType *type_a, struct ZenitType *type_b);

/*
 * Function: zenit_type_reference_is_assignable_from
 *  Checks if an object with type equals to *value_type* can be assigned to an object with a reference type
 *  equals to *target_type*
 *
 * Parameters:
 *  <struct ZenitReferenceType> *target_type: Target reference type for the assignment
 *  <struct ZenitType> *value_type: Source type of the assignment
 *
 * Returns:
 *  bool: *true* if an object of type *value_type* can be assigned to an object with type *target_type*,
 *        otherwise this function returns *false*.
 */
bool zenit_type_reference_is_assignable_from(struct ZenitReferenceType *target_type, struct ZenitType *value_type);

/*
 * Function: zenit_type_reference_is_castable_to
 *  Checks if an object with a reference type equals to *ref_type* can be casted to an object of type equals
 *  to *target_type*
 *
 * Parameters:
 *  <struct ZenitReferenceType> *ref_type: Source type of the cast
 *  <struct ZenitType> *target_type: Target type of the cast
 *
 * Returns:
 *  bool: *true* if the reference type can be casted to a *target_type*, otherwise *false*
 */
bool zenit_type_reference_is_castable_to(struct ZenitReferenceType *ref_type, struct ZenitType *target_type);

/*
 * Function: zenit_type_reference_can_unify
 *  Checks if the reference type object can be unified with the type represented by *type_b*
 *
 * Parameters:
 *  <struct ZenitReferenceType> *ref_type: Reference type object
 *  <struct ZenitType> *type_b: Type object
 *
 * Returns:
 *  bool: *true* if the types can be unified, which means they can both be represented by an ancestor
 *        or enclosing -base- type, otherwise *false*.
 */
bool zenit_type_reference_can_unify(struct ZenitReferenceType *ref_type, struct ZenitType *type_b);

/*
 * Function: zenit_type_reference_free
 *  Frees the memory of the reference type object
 *
 * Parameters:
 *  <struct ZenitReferenceType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_type_reference_free(struct ZenitReferenceType *type);

#endif /* ZENIT_TYPE_REFERENCE_H */
