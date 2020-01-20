#ifndef ZIR_TYPE_H
#define ZIR_TYPE_H

#include <stdlib.h>
#include <stdbool.h>

/*
 * Enum: enum ZirTypeKind
 *  Enumerates all the native type supported by Zir
 *  including some special values that the compiler
 *  uses internally
 *
 */
enum ZirTypeKind {
    ZIR_TYPE_NONE,
    ZIR_TYPE_REFERENCE,
    ZIR_TYPE_ARRAY,
    ZIR_TYPE_STRUCT,
    ZIR_TYPE_UINT,
    ZIR_TYPE_END
};

/*
 * Struct: struct ZirTypeString
 *  Contains a version of the string representation of a type object
 * 
 * Members:
 *  <unsigned long> version: Type's hash value
 *  <char> *: String value
 * 
 */
struct ZirTypeString {
    unsigned long version;
    char *value;
};

/*
 * Struct: struct ZirType
 *  Represents the information of a type. 
 * 
 * Members:
 *  <enum ZirTypeKind> type: The raw type.
 *  <struct ZirTypeString> to_string: Keeps track of the string representation of the type object
 *  
 */
struct ZirType {
    enum ZirTypeKind typekind;
    struct ZirTypeString to_string;
};

/*
 * Function: zir_type_hash
 *  Return a hash number for the current version of the type information object
 *
 * Parameters:
 *  <struct ZirType> *type: Type object
 *
 * Returns:
 *  <unsigned long>: Hash of the type object
 * 
 */
unsigned long zir_type_hash(struct ZirType *type);

/*
 * Function: zir_type_to_string
 *  Returns a string representation of the type.
 *
 * Parameters:
 *  <struct ZirType> *type: Type object.
 *
 * Returns:
 *  <char>*: String representation of the *type* object
 *
 */
char* zir_type_to_string(struct ZirType *type);

/*
 * Function: zir_type_equals
 *  Compares *type_a* and *type_b* to know if they are equals
 *
 * Parameters:
 *  <struct ZirType> *type_a: Object to compare
 *  <struct ZirType> *type_b: Object to compare
 *
 * Returns:
 *  <bool>: *true* if the objects are equals, otherwise *false*.
 *
 */
bool zir_type_equals(struct ZirType *type_a, struct ZirType *type_b);

/*
 * Function: zir_type_copy
 *  Returns a new object that is a copy of the source object
 *
 * Parameters:
 *  <struct ZirType> *src_type: Type object to be copied
 *
 * Returns:
 *  <struct ZirType>*: New object copied from the source object
 * 
 */
struct ZirType* zir_type_copy(struct ZirType *src_type);

/*
 * Function: zir_type_is_assignable_from
 *  Checks if the type represented by the *target_type* object accepts assignments from the type represented
 *  by the *value_type* object
 *
 * Parameters:
 *  <struct ZirType> *target_type: Target type of the assignment
 *  <struct ZirType> *value_type: Source type of the assignment
 *
 * Returns:
 *  <bool>: *true* if *target_type* accepts the *value_type*, otherwise, *false*.
 *
 */
bool zir_type_is_assignable_from(struct ZirType *target_type, struct ZirType *value_type);

/*
 * Function: zir_type_is_castable_to
 *  Checks if the type represented by the *source_type* object can be casted to the type represented by
 *  the *target_cast_type*
 *
 * Parameters:
 *  <struct ZirType> *source_type: Original type object
 *  <struct ZirType> *target_cast_type: Destination type object
 *
 * Returns:
 *  <bool>: *true* if the source type can be casted to the target type, otherwise, *false*.
 * 
 */
bool zir_type_is_castable_to(struct ZirType *source_type, struct ZirType *target_cast_type);

/*
 * Function: zir_type_size
 *  Returns the size needed to store an instance of the provided type (in bytes)
 *
 * Parameters:
 *  <struct ZirType> *type: Type object
 *
 * Returns:
 *  size_t: Size needed to store an instance of the type
 * 
 */
size_t zir_type_size(struct ZirType *type);

/*
 * Function: zir_type_fre
 *  Frees the memory allocated in the type object
 *
 * Parameters:
 *  <struct ZirType> *type: Type object to be freed
 *
 * Returns:
 *  void: This function does not return a value
 *
 * Notes:
 *  This function accepts any "descendant" of the "base" type information struct
 */
void zir_type_free(struct ZirType *type);

#endif /* ZIR_TYPE_H */
