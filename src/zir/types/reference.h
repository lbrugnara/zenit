#ifndef ZIR_TYPE_REFERENCE_H
#define ZIR_TYPE_REFERENCE_H

#include "type.h"

/*
 * Struct: struct ZirReferenceType
 *  Represents a reference type
 * 
 * Members:
 *  <struct ZirType> base: Base type information
 *  <struct ZirType> *element: The type of the referenced symbol
 */
struct ZirReferenceType {
    struct ZirType base;
    struct ZirType *element;
};

/*
 * Function: zir_type_reference_new
 *  Returns a new instance of a reference type
 *
 * Parameters:
 *  <struct ZirType> *element: A type object that represents the type of the referenced symbol
 *
 * Returns:
 *  struct ZirReferenceType*: Pointer to a a reference type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_type_reference_free> function
 */
struct ZirReferenceType* zir_type_reference_new(struct ZirType *element);

/*
 * Function: zir_type_reference_hash
 *  Returns a hash that identifies the reference type object
 *
 * Parameters:
 *  <struct ZirReferenceType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zir_type_reference_hash(struct ZirReferenceType *type);

/*
 * Function: zir_type_reference_to_string
 *  Returns a string representation of the reference type object
 *
 * Parameters:
 *  <struct ZirReferenceType> *type: Type object
 *
 * Returns:
 *  char*: String representation of the reference type
 *
 * Notes:
 *  The string returned by this function MUST NOT be freed by the caller, the type object
 *  has ownership of it, and it frees the string memory when the function <zir_type_reference_free>
 *  is used to free the memory used by the type object.
 */
char* zir_type_reference_to_string(struct ZirReferenceType *type);

/*
 * Function: zir_type_reference_equals
 *  Check if the reference type *type_a* is equals to the *type_b* object
 *
 * Parameters:
 *  <struct ZirReferenceType> *type_a: Reference type to compare 
 *  <struct ZirType> *type_b: Type to compare against the reference type
 *
 * Returns:
 *  bool: *true* if *type_b* is a reference type equals to *type_a*, otherwise it returns *false*
 */
bool zir_type_reference_equals(struct ZirReferenceType *type_a, struct ZirType *type_b);

/*
 * Function: zir_type_reference_is_assignable_from
 *  Checks if an object with type equals to *value_type* can be assigned to an object with a reference type
 *  equals to *target_type*
 *
 * Parameters:
 *  <struct ZirReferenceType> *target_type: Target reference type for the assignment
 *  <struct ZirType> *value_type: Source type of the assignment
 *
 * Returns:
 *  bool: *true* if an object of type *value_type* can be assigned to an object with type *target_type*,
 *        otherwise this function returns *false*.
 */
bool zir_type_reference_is_assignable_from(struct ZirReferenceType *target_type, struct ZirType *value_type);

/*
 * Function: zir_type_reference_size
 *  Returns the size needed to store an instance of the reference type (in bytes)
 *
 * Parameters:
 *  <struct ZirReferenceType> *type: Reference type object
 *
 * Returns:
 *  size_t: Size needed to store an instance of the reference type
 */
size_t zir_type_reference_size(struct ZirReferenceType *type);

/*
 * Function: zir_type_reference_is_castable_to
 *  Checks if an object with a reference type equals to *ref_type* can be casted to an object of type equals
 *  to *target_type*
 *
 * Parameters:
 *  <struct ZirReferenceType> *ref_type: Source type of the cast
 *  <struct ZirType> *target_type: Target type of the cast
 *
 * Returns:
 *  bool: *true* if the reference type can be casted to a *target_type*, otherwise *false*
 */
bool zir_type_reference_is_castable_to(struct ZirReferenceType *ref_type, struct ZirType *target_type);

/*
 * Function: zir_type_reference_free
 *  Frees the memory of the reference type object
 *
 * Parameters:
 *  <struct ZirReferenceType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_type_reference_free(struct ZirReferenceType *type);

#endif /* ZIR_TYPE_REFERENCE_H */
