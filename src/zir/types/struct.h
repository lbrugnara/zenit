#ifndef ZIR_TYPE_STRUCT_H
#define ZIR_TYPE_STRUCT_H


#include <fllib/containers/List.h>
#include "type.h"

/*
 * Struct: ZirStructTypeMember
 *  Represents a member of an struct type object
 * 
 * Members:
 *  <const char> *name: Member name
 *  <ZirType> *type: Type of the member
 * 
 */
typedef struct ZirStructTypeMember {
    const char *name;
    ZirType *type;
} ZirStructTypeMember;

/*
 * Struct: ZirStructType
 *  Represents a struct type
 * 
 * Members:
 *  <ZirType> base: Base type information
 *  <char> *name: The name of the struct if it is a named struct or <NULL> for unnamed structs
 *  <FlList> *members: List of <ZirStructTypeMember> objects that represents each struct member
 */
typedef struct ZirStructType {
    ZirType base;
    char *name;
    FlList *members;
} ZirStructType;

/*
 * Function: zir_struct_type_new
 *  Returns a new instance of a struct type
 *
 * Parameters:
 *  <char> *name: A pointer to a string that represents the name of the struct, or NULL if it is an unnamed struct
 *
 * Returns:
 *  ZirStructType*: Pointer to a a struct type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_struct_type_free> function
 */
ZirStructType* zir_struct_type_new(char *name);

/*
 * Function: zir_struct_type_add_member
 * 
 *  Adds a member to the list of members of the struct type
 *
 * Parameters:
 *  <strict ZirStructType> *struct_type: Struct type object
 *  <const char> *name: Name of the member. It cannot be NULL
 *  <ZirType> *type: Type of the member
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_struct_type_add_member(ZirStructType *struct_type, const char *name, ZirType *member_type);

/*
 * Function: zir_struct_type_get_member
 *  Returns -if it exists- the struct type member matching its name with the provided *name*.
 *
 * Parameters:
 *  <ZirStructType> *struct_type: Struct type object
 *  <const char> *name: The member name
 *
 * Returns:
 *  ZirStructTypeMember*: The struct type member or NULL if it doesn't exist
 */
ZirStructTypeMember* zir_struct_type_get_member(ZirStructType *struct_type, const char *name);

/*
 * Function: zir_struct_type_hash
 *  Returns a hash that identifies the struct type object
 *
 * Parameters:
 *  <ZirStructType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zir_struct_type_hash(ZirStructType *type);

/*
 * Function: zir_struct_type_to_string
 *  Returns a string representation of the struct type object
 *
 * Parameters:
 *  <ZirStructType> *type: Type object
 *
 * Returns:
 *  char*: String representation of the struct type
 *
 * Notes:
 *  The string returned by this function MUST NOT be freed by the caller, the type object
 *  has ownership of it, and it frees the string memory when the function <zir_struct_type_free>
 *  is used to free the memory used by the type object.
 */
char* zir_struct_type_to_string(ZirStructType *type);

/*
 * Function: zir_struct_type_equals
 *  Check if the struct type *type_a* is equals to the *type_b* object
 *
 * Parameters:
 *  <ZirStructType> *type_a: Struct type to compare 
 *  <ZirType> *type_b: Type to compare against the struct type
 *
 * Returns:
 *  bool: *true* if *type_b* is a struct type equals to *type_a*, otherwise it returns *false*
 */
bool zir_struct_type_equals(ZirStructType *type_a, ZirType *type_b);

/*
 * Function: zir_struct_type_structurally_equals
 *  Check if the struct type *type_a* is structurally equals to the struct type *type_b*
 *
 * Parameters:
 *  <ZirStructType> *type_a: Struct type to compare 
 *  <ZirStructType> *type_b: Struct type to compare 
 *
 * Returns:
 *  bool: *true* if structs typs *type_a* and *type_b* are structurally equals
 */
bool zir_struct_type_structurally_equals(ZirStructType *type_a, ZirStructType *type_b);

/*
 * Function: zir_struct_type_is_assignable_from
 *  Checks if an object with type equals to *value_type* can be assigned to an object with a struct type
 *  equals to *target_type*
 *
 * Parameters:
 *  <ZirStructType> *target_type: Target struct type for the assignment
 *  <ZirType> *value_type: Source type of the assignment
 *
 * Returns:
 *  bool: *true* if an object of type *value_type* can be assigned to an object with type *target_type*,
 *        otherwise this function returns *false*.
 */
bool zir_struct_type_is_assignable_from(ZirStructType *target_type, ZirType *value_type);

/*
 * Function: zir_struct_type_is_castable_to
 *  Checks if an object with a struct type equals to *struct_type* can be casted to an object of type equals
 *  to *target_type*
 *
 * Parameters:
 *  <ZirStructType> *struct_type: Source type of the cast
 *  <ZirType> *target_type: Target type of the cast
 *
 * Returns:
 *  bool: *true* if the struct type can be casted to a *target_type*, otherwise *false*
 */
bool zir_struct_type_is_castable_to(ZirStructType *struct_type, ZirType *target_type);

/*
 * Function: zir_struct_type_size
 *  Returns the size needed to store an instance of the struct type (in bytes)
 *
 * Parameters:
 *  <ZirStructType> *type: Struct type object
 *
 * Returns:
 *  size_t: Size needed to store an instance of the struct type
 */
size_t zir_struct_type_size(ZirStructType *type, size_t ref_size);

/*
 * Function: zir_struct_type_free
 *  Frees the memory of the struct type object
 *
 * Parameters:
 *  <ZirStructType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_struct_type_free(ZirStructType *type);

#endif /* ZIR_TYPE_STRUCT_H */
