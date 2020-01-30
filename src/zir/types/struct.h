#ifndef ZIR_TYPE_STRUCT_H
#define ZIR_TYPE_STRUCT_H

#include <fllib.h>
#include "type.h"

/*
 * Struct: struct ZirStructTypeMember
 *  Represents a member of an struct type object
 * 
 * Members:
 *  <const char> *name: Member name
 *  <struct ZirType> *type: Type of the member
 * 
 */
struct ZirStructTypeMember {
    const char *name;
    struct ZirType *type;
};

/*
 * Struct: struct ZirStructType
 *  Represents a struct type
 * 
 * Members:
 *  <struct ZirType> base: Base type information
 *  <char> *name: The name of the struct if it is a named struct or <NULL> for unnamed structs
 *  <FlList> *members: List of <struct ZirStructTypeMember> objects that represents each struct member
 */
struct ZirStructType {
    struct ZirType base;
    char *name;
    FlList *members;
};

/*
 * Function: zir_type_struct_new
 *  Returns a new instance of a struct type
 *
 * Parameters:
 *  <char> *name: A pointer to a string that represents the name of the struct, or NULL if it is an unnamed struct
 *
 * Returns:
 *  struct ZirStructType*: Pointer to a a struct type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_type_struct_free> function
 */
struct ZirStructType* zir_type_struct_new(char *name);

/*
 * Function: zir_type_struct_add_member
 * 
 *  Adds a member to the list of members of the struct type
 *
 * Parameters:
 *  <strict ZirStructType> *struct_type: Struct type object
 *  <const char> *name: Name of the member. It cannot be NULL
 *  <struct ZirType> *type: Type of the member
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_type_struct_add_member(struct ZirStructType *struct_type, const char *name, struct ZirType *member_type);

/*
 * Function: zir_type_struct_get_member
 *  Returns -if it exists- the struct type member matching its name with the provided *name*.
 *
 * Parameters:
 *  <struct ZirStructType> *struct_type: Struct type object
 *  <const char> *name: The member name
 *
 * Returns:
 *  struct ZirStructTypeMember*: The struct type member or NULL if it doesn't exist
 */
struct ZirStructTypeMember* zir_type_struct_get_member(struct ZirStructType *struct_type, const char *name);

/*
 * Function: zir_type_struct_hash
 *  Returns a hash that identifies the struct type object
 *
 * Parameters:
 *  <struct ZirStructType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zir_type_struct_hash(struct ZirStructType *type);

/*
 * Function: zir_type_struct_to_string
 *  Returns a string representation of the struct type object
 *
 * Parameters:
 *  <struct ZirStructType> *type: Type object
 *
 * Returns:
 *  char*: String representation of the struct type
 *
 * Notes:
 *  The string returned by this function MUST NOT be freed by the caller, the type object
 *  has ownership of it, and it frees the string memory when the function <zir_type_struct_free>
 *  is used to free the memory used by the type object.
 */
char* zir_type_struct_to_string(struct ZirStructType *type);

/*
 * Function: zir_type_struct_equals
 *  Check if the struct type *type_a* is equals to the *type_b* object
 *
 * Parameters:
 *  <struct ZirStructType> *type_a: Struct type to compare 
 *  <struct ZirType> *type_b: Type to compare against the struct type
 *
 * Returns:
 *  bool: *true* if *type_b* is a struct type equals to *type_a*, otherwise it returns *false*
 */
bool zir_type_struct_equals(struct ZirStructType *type_a, struct ZirType *type_b);

/*
 * Function: zir_type_struct_structurally_equals
 *  Check if the struct type *type_a* is structurally equals to the struct type *type_b*
 *
 * Parameters:
 *  <struct ZirStructType> *type_a: Struct type to compare 
 *  <struct ZirStructType> *type_b: Struct type to compare 
 *
 * Returns:
 *  bool: *true* if structs typs *type_a* and *type_b* are structurally equals
 */
bool zir_type_struct_structurally_equals(struct ZirStructType *type_a, struct ZirStructType *type_b);

/*
 * Function: zir_type_struct_is_assignable_from
 *  Checks if an object with type equals to *value_type* can be assigned to an object with a struct type
 *  equals to *target_type*
 *
 * Parameters:
 *  <struct ZirStructType> *target_type: Target struct type for the assignment
 *  <struct ZirType> *value_type: Source type of the assignment
 *
 * Returns:
 *  bool: *true* if an object of type *value_type* can be assigned to an object with type *target_type*,
 *        otherwise this function returns *false*.
 */
bool zir_type_struct_is_assignable_from(struct ZirStructType *target_type, struct ZirType *value_type);

/*
 * Function: zir_type_struct_is_castable_to
 *  Checks if an object with a struct type equals to *struct_type* can be casted to an object of type equals
 *  to *target_type*
 *
 * Parameters:
 *  <struct ZirStructType> *struct_type: Source type of the cast
 *  <struct ZirType> *target_type: Target type of the cast
 *
 * Returns:
 *  bool: *true* if the struct type can be casted to a *target_type*, otherwise *false*
 */
bool zir_type_struct_is_castable_to(struct ZirStructType *struct_type, struct ZirType *target_type);

/*
 * Function: zir_type_struct_size
 *  Returns the size needed to store an instance of the struct type (in bytes)
 *
 * Parameters:
 *  <struct ZirStructType> *type: Struct type object
 *
 * Returns:
 *  size_t: Size needed to store an instance of the struct type
 */
size_t zir_type_struct_size(struct ZirStructType *type);

/*
 * Function: zir_type_struct_free
 *  Frees the memory of the struct type object
 *
 * Parameters:
 *  <struct ZirStructType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_type_struct_free(struct ZirStructType *type);

#endif /* ZIR_TYPE_STRUCT_H */
