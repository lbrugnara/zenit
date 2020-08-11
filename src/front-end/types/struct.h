#ifndef ZENIT_TYPE_STRUCT_H
#define ZENIT_TYPE_STRUCT_H


#include <fllib/containers/List.h>
#include "type.h"

/*
 * Struct: ZenitStructTypeMember
 *  Represents a member of an struct type object
 * 
 * Members:
 *  <const char> *name: Member name
 *  <ZenitType> *type: Type of the member
 * 
 */
typedef struct ZenitStructTypeMember {
    const char *name;
    ZenitType *type;
} ZenitStructTypeMember;

/*
 * Struct: ZenitStructType
 *  Represents a struct type
 * 
 * Members:
 *  <ZenitType> base: Base type information
 *  <char> *name: The name of the struct if it is a named struct or <NULL> for unnamed structs
 *  <FlList> *members: List of <ZenitStructTypeMember> objects that represents each struct member
 */
typedef struct ZenitStructType {
    ZenitType base;
    char *name;
    FlList *members;
} ZenitStructType;

/*
 * Function: zenit_struct_type_new
 *  Returns a new instance of a struct type
 *
 * Parameters:
 *  <char> *name: A pointer to a string that represents the name of the struct, or NULL if it is an unnamed struct
 *
 * Returns:
 *  ZenitStructType*: Pointer to a a struct type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_struct_type_free> function
 */
ZenitStructType* zenit_struct_type_new(char *name);

/*
 * Function: zenit_struct_type_add_member
 * 
 *  Adds a member to the list of members of the struct type
 *
 * Parameters:
 *  <strict ZenitStructType> *struct_type: Struct type object
 *  <const char> *name: Name of the member. It cannot be NULL
 *  <ZenitType> *type: Type of the member
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_struct_type_add_member(ZenitStructType *struct_type, const char *name, ZenitType *member_type);

/*
 * Function: zenit_struct_type_get_member
 *  Returns -if it exists- the struct type member matching its name with the provided *name*.
 *
 * Parameters:
 *  <ZenitStructType> *struct_type: Struct type object
 *  <const char> *name: The member name
 *
 * Returns:
 *  ZenitStructTypeMember*: The struct type member or NULL if it doesn't exist
 */
ZenitStructTypeMember* zenit_struct_type_get_member(ZenitStructType *struct_type, const char *name);

/*
 * Function: zenit_struct_type_hash
 *  Returns a hash that identifies the struct type object
 *
 * Parameters:
 *  <ZenitStructType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zenit_struct_type_hash(ZenitStructType *type);

/*
 * Function: zenit_struct_type_to_string
 *  Returns a string representation of the struct type object
 *
 * Parameters:
 *  <ZenitStructType> *type: Type object
 *
 * Returns:
 *  char*: String representation of the struct type
 *
 * Notes:
 *  The string returned by this function MUST NOT be freed by the caller, the type object
 *  has ownership of it, and it frees the string memory when the function <zenit_struct_type_free>
 *  is used to free the memory used by the type object.
 */
char* zenit_struct_type_to_string(ZenitStructType *type);

/*
 * Function: zenit_struct_type_equals
 *  Check if the struct type *type_a* is equals to the *type_b* object
 *
 * Parameters:
 *  <ZenitStructType> *type_a: Struct type to compare 
 *  <ZenitType> *type_b: Type to compare against the struct type
 *
 * Returns:
 *  bool: *true* if *type_b* is a struct type equals to *type_a*, otherwise it returns *false*
 */
bool zenit_struct_type_equals(ZenitStructType *type_a, ZenitType *type_b);

/*
 * Function: zenit_struct_type_structurally_equals
 *  Check if the struct type *type_a* is structurally equals to the struct type *type_b*
 *
 * Parameters:
 *  <ZenitStructType> *type_a: Struct type to compare 
 *  <ZenitStructType> *type_b: Struct type to compare 
 *
 * Returns:
 *  bool: *true* if structs typs *type_a* and *type_b* are structurally equals
 */
bool zenit_struct_type_structurally_equals(ZenitStructType *type_a, ZenitStructType *type_b);

/*
 * Function: zenit_struct_type_is_assignable_from
 *  Checks if an object with type equals to *value_type* can be assigned to an object with a struct type
 *  equals to *target_type*
 *
 * Parameters:
 *  <ZenitStructType> *target_type: Target struct type for the assignment
 *  <ZenitType> *value_type: Source type of the assignment
 *
 * Returns:
 *  bool: *true* if an object of type *value_type* can be assigned to an object with type *target_type*,
 *        otherwise this function returns *false*.
 */
bool zenit_struct_type_is_assignable_from(ZenitStructType *target_type, ZenitType *value_type);

/*
 * Function: zenit_struct_type_is_castable_to
 *  Checks if an object with a struct type equals to *struct_type* can be casted to an object of type equals
 *  to *target_type*
 *
 * Parameters:
 *  <ZenitStructType> *struct_type: Source type of the cast
 *  <ZenitType> *target_type: Target type of the cast
 *
 * Returns:
 *  bool: *true* if the struct type can be casted to a *target_type*, otherwise *false*
 */
bool zenit_struct_type_is_castable_to(ZenitStructType *struct_type, ZenitType *target_type);

/*
 * Function: zenit_struct_type_can_unify
 *  Checks if the struct type object can be unified with the type represented by *type_b*
 *
 * Parameters:
 *  <ZenitStructType> *struct_type: Struct type object
 *  <ZenitType> *type_b: Type object
 *
 * Returns:
 *  bool: *true* if the types can be unified, which means they can both be represented by an ancestor
 *        or enclosing -base- type, otherwise *false*.
 */
bool zenit_struct_type_can_unify(ZenitStructType *struct_type, ZenitType *type_b);

/*
 * Function: zenit_struct_type_free
 *  Frees the memory of the struct type object
 *
 * Parameters:
 *  <ZenitStructType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_struct_type_free(ZenitStructType *type);

#endif /* ZENIT_TYPE_STRUCT_H */
