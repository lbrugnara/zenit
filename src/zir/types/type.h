#ifndef ZIR_TYPE_H
#define ZIR_TYPE_H

#include <stdbool.h>
#include <stdlib.h>

/*
 * Enum: ZirTypeKind
 *  Enumerates all the native type supported by Zir
 *  including some special values that the compiler
 *  uses internally
 *
 */
typedef enum ZirTypeKind {
    ZIR_TYPE_NONE,
    ZIR_TYPE_UINT,
    ZIR_TYPE_BOOL,
    ZIR_TYPE_ARRAY,
    ZIR_TYPE_STRUCT,
    ZIR_TYPE_REFERENCE,
} ZirTypeKind;

/*
 * Struct: ZirTypeString
 *  Contains a version of the string representation of a type object
 * 
 * Members:
 *  <unsigned long> version: Type's hash value
 *  <char> *: String value
 * 
 */
typedef struct ZirTypeString {
    unsigned long version;
    char *value;
} ZirTypeString;

/*
 * Struct: ZirType
 *  It is the "base type" of all the Zir type objects that contains
 *  information that is shared between all the different types
 * 
 * Members:
 *  <ZirTypeKind> typekind: The native kind of this type
 *  <ZirTypeString> to_string: String representation of the type
 * 
 * Notes:
 *  The *to_string* property is populated when a call to the function zir_type_to_string occurs (or to any of its
 *  specializations) and shouldn't be directly manipulated
 * 
 */
typedef struct ZirType {
    ZirTypeKind typekind;
    ZirTypeString to_string;
} ZirType;

/*
 * Function: zir_type_hash
 *  Return a hash number for the current version of the type information object
 *
 * Parameters:
 *  <ZirType> *type: Type object
 *
 * Returns:
 *  <unsigned long>: Hash of the type object
 */
unsigned long zir_type_hash(ZirType *type);

/*
 * Function: zir_type_to_string
 *  Returns a string representation of the type.
 *
 * Parameters:
 *  <ZirType> *type: Type object.
 *
 * Returns:
 *  <char>*: String representation of the *type* object
 */
char* zir_type_to_string(ZirType *type);

/*
 * Function: zir_type_equals
 *  Compares *type_a* and *type_b* to know if they are equals
 *
 * Parameters:
 *  <ZirType> *type_a: Object to compare
 *  <ZirType> *type_b: Object to compare
 *
 * Returns:
 *  <bool>: *true* if the objects are equals, otherwise *false*.
 */
bool zir_type_equals(ZirType *type_a, ZirType *type_b);

/*
 * Function: zir_type_is_assignable_from
 *  Checks if the type represented by the *target_type* object accepts assignments from the type represented
 *  by the *value_type* object
 *
 * Parameters:
 *  <ZirType> *target_type: Target type of the assignment
 *  <ZirType> *value_type: Source type of the assignment
 *
 * Returns:
 *  <bool>: *true* if *target_type* accepts the *value_type*, otherwise, *false*.
 */
bool zir_type_is_assignable_from(ZirType *target_type, ZirType *value_type);

/*
 * Function: zir_type_is_castable_to
 *  Checks if the type represented by the *source_type* object can be casted to the type represented by
 *  the *target_cast_type*
 *
 * Parameters:
 *  <ZirType> *source_type: Original type object
 *  <ZirType> *target_cast_type: Destination type object
 *
 * Returns:
 *  <bool>: *true* if the source type can be casted to the target type, otherwise, *false*.
 */
bool zir_type_is_castable_to(ZirType *source_type, ZirType *target_cast_type);

/*
 * Function: zir_type_size
 *  Returns the size needed to store an instance of the provided type (in bytes)
 *
 * Parameters:
 *  <ZirType> *type: Type object
 *  <size_t> ref_size: The default size of pointers (back-end specific)
 *
 * Returns:
 *  size_t: Size needed to store an instance of the type
 */
size_t zir_type_size(ZirType *type, size_t ref_size);

/*
 * Function: zir_type_fre
 *  Frees the memory allocated in the type object
 *
 * Parameters:
 *  <ZirType> *type: Type object to be freed
 *
 * Returns:
 *  void: This function does not return a value
 *
 * Notes:
 *  This function accepts any "descendant" of the "base" type information struct
 */
void zir_type_free(ZirType *type);

#endif /* ZIR_TYPE_H */
