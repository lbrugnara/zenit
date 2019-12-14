#ifndef ZIR_TYPE_H
#define ZIR_TYPE_H

#include <stdlib.h>
#include <stdbool.h>

/*
 * Enum: enum ZirType
 *  Enumerates all the native type supported by Zir
 *  including some special values that the compiler
 *  uses internally
 *
 */
enum ZirType {
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
 * Struct: struct ZirTypeInfo
 *  Represents the information of a type. 
 * 
 * Members:
 *  <enum ZirType> type: The raw type.
 *  <struct ZirTypeString> to_string: Keeps track of the string representation of the type object
 *  
 */
struct ZirTypeInfo {
    enum ZirType type;
    struct ZirTypeString to_string;
};

/*
 * Function: zir_type_hash
 *  Return a hash number for the current version of the type information object
 *
 * Parameters:
 *  <struct ZirTypeInfo> *typeinfo: Type object
 *
 * Returns:
 *  <unsigned long>: Hash of the type object
 * 
 */
unsigned long zir_type_hash(struct ZirTypeInfo *typeinfo);

/*
 * Function: zir_type_to_string
 *  Returns a string representation of the type.
 *
 * Parameters:
 *  <struct ZirTypeInfo> *typeinfo: Type object.
 *
 * Returns:
 *  <char>*: String representation of the *typeinfo* object
 *
 */
char* zir_type_to_string(struct ZirTypeInfo *typeinfo);

/*
 * Function: zir_type_equals
 *  Compares *type_a* and *type_b* to know if they are equals
 *
 * Parameters:
 *  <struct ZirTypeInfo> *type_a: Object to compare
 *  <struct ZirTypeInfo> *type_b: Object to compare
 *
 * Returns:
 *  <bool>: *true* if the objects are equals, otherwise *false*.
 *
 */
bool zir_type_equals(struct ZirTypeInfo *type_a, struct ZirTypeInfo *type_b);

/*
 * Function: zir_type_copy
 *  Returns a new object that is a copy of the source object
 *
 * Parameters:
 *  <struct ZirTypeInfo> *src_type: Type object to be copied
 *
 * Returns:
 *  <struct ZirTypeInfo>*: New object copied from the source object
 * 
 */
struct ZirTypeInfo* zir_type_copy(struct ZirTypeInfo *src_type);

/*
 * Function: zir_type_unify
 *  Searches for a common ancestor between *type_a* and *type_b*.
 *
 * Parameters:
 *  <struct ZirTypeInfo> *type_a: Type object
 *  <struct ZirTypeInfo> *type_b: Type object
 *  <struct ZirTypeInfo> **unified: If a common ancestor between A and B exists, and *unified* is not NULL, a
 *   copy of the common ancestor will be allocated in the pointer pointed by this parameter.
 * 
 * Returns:
 *  <bool>: *true* if a common ancestor between types A and B exists, otherwise, this function returns *false*.
 *
 * Notes:
 *  If *unified* is NULL, this function does not allocate memory and it just returns *true* or *false*.
 */
bool zir_type_unify(struct ZirTypeInfo *type_a, struct ZirTypeInfo *type_b, struct ZirTypeInfo **unified);

/*
 * Function: zir_type_is_assignable_from
 *  Checks if the type represented by the *target_type* object accepts assignments from the type represented
 *  by the *value_type* object
 *
 * Parameters:
 *  <struct ZirTypeInfo> *target_type: Target type of the assignment
 *  <struct ZirTypeInfo> *value_type: Source type of the assignment
 *
 * Returns:
 *  <bool>: *true* if *target_type* accepts the *value_type*, otherwise, *false*.
 *
 */
bool zir_type_is_assignable_from(struct ZirTypeInfo *target_type, struct ZirTypeInfo *value_type);

/*
 * Function: zir_type_is_castable_to
 *  Checks if the type represented by the *source_type* object can be casted to the type represented by
 *  the *target_cast_type*
 *
 * Parameters:
 *  <struct ZirTypeInfo> *source_type: Original type object
 *  <struct ZirTypeInfo> *target_cast_type: Destination type object
 *
 * Returns:
 *  <bool>: *true* if the source type can be casted to the target type, otherwise, *false*.
 * 
 */
bool zir_type_is_castable_to(struct ZirTypeInfo *source_type, struct ZirTypeInfo *target_cast_type);

size_t zir_type_size(struct ZirTypeInfo *type);

/*
 * Function: zir_type_fre
 *  Frees the memory allocated in the type object
 *
 * Parameters:
 *  <struct ZirTypeInfo> *typeinfo: Type object to be freed
 *
 * Returns:
 *  void: This function does not return a value
 *
 * Notes:
 *  This function accepts any "descendant" of the "base" type information struct
 */
void zir_type_free(struct ZirTypeInfo *typeinfo);

#endif /* ZIR_TYPE_H */
