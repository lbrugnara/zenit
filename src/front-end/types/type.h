#ifndef ZENIT_TYPE_H
#define ZENIT_TYPE_H

#include <fllib/Slice.h>

/*
 * Enum: ZenitTypeKind
 *  Enumerates all the native type supported by Zenit
 *  including some special values that the compiler
 *  uses internally
 *
 */
typedef enum ZenitTypeKind {
    ZENIT_TYPE_NONE,
    ZENIT_TYPE_UINT,
    ZENIT_TYPE_BOOL,
    ZENIT_TYPE_ARRAY,
    ZENIT_TYPE_STRUCT,
    ZENIT_TYPE_REFERENCE,
} ZenitTypeKind;

/*
 * Struct: ZenitTypeString
 *  Contains a version of the string representation of a type object
 * 
 * Members:
 *  <unsigned long> version: Type's hash value
 *  <char> *: String value
 * 
 */
typedef struct ZenitTypeString {
    unsigned long version;
    char *value;
} ZenitTypeString;

/*
 * Struct: ZenitType
 *  It is the "base type" of all the Zenit type objects that contains
 *  information that is shared between all the different types
 * 
 * Members:
 *  <ZenitTypeKind> typekind: The native kind of this type
 *  <ZenitTypeString> to_string: String representation of the type
 * 
 * Notes:
 *  The *to_string* property is populated when a call to the function zenit_type_to_string occurs (or to any of its
 *  specializations) and shouldn't be directly manipulated
 * 
 */
typedef struct ZenitType {
    ZenitTypeKind typekind;
    ZenitTypeString to_string;
} ZenitType;

/*
 * Function: zenit_type_hash
 *  Return a hash number for the current version of the type information object
 *
 * Parameters:
 *  <ZenitType> *type: Type object
 *
 * Returns:
 *  <unsigned long>: Hash of the type object
 */
unsigned long zenit_type_hash(ZenitType *type);

/*
 * Function: zenit_type_from_slice
 *  Takes a string slice and returns the system type for that representation.
 *
 * Parameters:
 *  <struct FlSlice> *slice: Sequence of bytes to match against the different system types
 *
 * Returns:
 *  <ZenitTypeKind>: The type that matches the sequence of bytes
 */
ZenitTypeKind zenit_type_from_slice(struct FlSlice *slice);

/*
 * Function: zenit_type_to_string
 *  Returns a string representation of the type.
 *
 * Parameters:
 *  <ZenitType> *type: Type object.
 *
 * Returns:
 *  <char>*: String representation of the *type* object
 */
char* zenit_type_to_string(ZenitType *type);

/*
 * Function: zenit_type_equals
 *  Compares *type_a* and *type_b* to know if they are equals
 *
 * Parameters:
 *  <ZenitType> *type_a: Object to compare
 *  <ZenitType> *type_b: Object to compare
 *
 * Returns:
 *  <bool>: *true* if the objects are equals, otherwise *false*.
 */
bool zenit_type_equals(ZenitType *type_a, ZenitType *type_b);

/*
 * Function: zenit_type_can_unify
 *  Checks if the type *type_a* can be unified with the type *type_b*
 *
 * Parameters:
 *  <ZenitUintType> *type_a: Type object
 *  <ZenitType> *type_b: Type object
 *
 * Returns:
 *  bool: *true* if the types can be unified, which means they can both be represented by an ancestor
 *        or enclosing -base- type, otherwise *false*.
 */
bool zenit_type_can_unify(ZenitType *type_a, ZenitType *type_b);

/*
 * Function: zenit_type_is_assignable_from
 *  Checks if the type represented by the *target_type* object accepts assignments from the type represented
 *  by the *value_type* object
 *
 * Parameters:
 *  <ZenitType> *target_type: Target type of the assignment
 *  <ZenitType> *value_type: Source type of the assignment
 *
 * Returns:
 *  <bool>: *true* if *target_type* accepts the *value_type*, otherwise, *false*.
 */
bool zenit_type_is_assignable_from(ZenitType *target_type, ZenitType *value_type);

/*
 * Function: zenit_type_is_castable_to
 *  Checks if the type represented by the *source_type* object can be casted to the type represented by
 *  the *target_cast_type*
 *
 * Parameters:
 *  <ZenitType> *source_type: Original type object
 *  <ZenitType> *target_cast_type: Destination type object
 *
 * Returns:
 *  <bool>: *true* if the source type can be casted to the target type, otherwise, *false*.
 */
bool zenit_type_is_castable_to(ZenitType *source_type, ZenitType *target_cast_type);

/*
 * Function: zenit_type_fre
 *  Frees the memory allocated in the type object
 *
 * Parameters:
 *  <ZenitType> *type: Type object to be freed
 *
 * Returns:
 *  void: This function does not return a value
 *
 * Notes:
 *  This function accepts any "descendant" of the "base" type information struct
 */
void zenit_type_free(ZenitType *type);

#endif /* ZENIT_TYPE_H */
