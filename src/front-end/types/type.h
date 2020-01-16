#ifndef ZENIT_TYPE_H
#define ZENIT_TYPE_H

/*
 * Enum: enum ZenitTypeKind
 *  Enumerates all the native type supported by Zenit
 *  including some special values that the compiler
 *  uses internally
 *
 */
enum ZenitTypeKind {
    ZENIT_TYPE_NONE,

    ZENIT_TYPE_REFERENCE,

    ZENIT_TYPE_ARRAY,

    ZENIT_TYPE_STRUCT,

    ZENIT_TYPE_UINT,

    ZENIT_TYPE_END
};

/*
 * Struct: struct ZenitTypeString
 *  Contains a version of the string representation of a type object
 * 
 * Members:
 *  <unsigned long> version: Type's hash value
 *  <char> *: String value
 * 
 */
struct ZenitTypeString {
    unsigned long version;
    char *value;
};

struct ZenitType {
    enum ZenitTypeKind typekind;
    struct ZenitTypeString to_string;
};

/*
 * Enum: enum ZenitTypeSource
 *  Identifies the source of the type information
 *  present in a <struct ZenitTypeInfo> object
 *
 * Members:
 *  <ZENIT_TYPE_SRC_INTRINSIC> means the type information is intrinsic to the language construction being used, ex: an integer
 *  <ZENIT_TYPE_SRC_HINT> is used to inform to the user that the type information is being obtained from the type hint present in the expression ex: var a : uint8 = 0;
 *  <ZENIT_TYPE_SRC_INFERRED> this is used when the type information is inferred from its usage ex: var a = 0; // a is a uint8
 */
enum ZenitTypeSource {
    ZENIT_TYPE_SRC_INTRINSIC,
    ZENIT_TYPE_SRC_HINT,
    ZENIT_TYPE_SRC_INFERRED,
};

/*
 * Struct: struct ZenitTypeInfo
 *  Represents the information of a type. 
 * 
 * Members:
 *  <enum ZenitTypeKind> type: The raw type.
 *  <enum ZenitTypeSource> source: The source of the information present in the type object
 *  <struct ZenitTypeString> to_string: Keeps track of the string representation of the type object
 *  
 */
struct ZenitTypeInfo {
    struct ZenitType *type;
    enum ZenitTypeSource source;
    bool sealed;
};

/*
 * Function: zenit_type_hash
 *  Return a hash number for the current version of the type information object
 *
 * Parameters:
 *  <struct ZenitType> *typeinfo: Type object
 *
 * Returns:
 *  <unsigned long>: Hash of the type object
 * 
 */
unsigned long zenit_type_hash(struct ZenitType *type);

/*
 * Function: zenit_type_from_slice
 *  Takes a string slice and returns the system type for that representation.
 *
 * Parameters:
 *  <struct FlSlice> *slice: Sequence of bytes to match against the different system types
 *
 * Returns:
 *  <enum ZenitTypeKind>: The type that matches the sequence of bytes
 *
 */
enum ZenitTypeKind zenit_type_from_slice(struct FlSlice *slice);

/*
 * Function: zenit_type_to_string
 *  Returns a string representation of the type.
 *
 * Parameters:
 *  <struct ZenitType> *type: Type object.
 *
 * Returns:
 *  <char>*: String representation of the *type* object
 *
 */
char* zenit_type_to_string(struct ZenitType *type);

/*
 * Function: zenit_type_equals
 *  Compares *type_a* and *type_b* to know if they are equals
 *
 * Parameters:
 *  <struct ZenitType> *type_a: Object to compare
 *  <struct ZenitType> *type_b: Object to compare
 *
 * Returns:
 *  <bool>: *true* if the objects are equals, otherwise *false*.
 *
 */
bool zenit_type_equals(struct ZenitType *type_a, struct ZenitType *type_b);

bool zenit_type_can_unify(struct ZenitType *type_a, struct ZenitType *type_b);

/*
 * Function: zenit_type_is_assignable_from
 *  Checks if the type represented by the *target_type* object accepts assignments from the type represented
 *  by the *value_type* object
 *
 * Parameters:
 *  <struct ZenitType> *target_type: Target type of the assignment
 *  <struct ZenitType> *value_type: Source type of the assignment
 *
 * Returns:
 *  <bool>: *true* if *target_type* accepts the *value_type*, otherwise, *false*.
 *
 */
bool zenit_type_is_assignable_from(struct ZenitType *target_type, struct ZenitType *value_type);

/*
 * Function: zenit_type_is_castable_to
 *  Checks if the type represented by the *source_type* object can be casted to the type represented by
 *  the *target_cast_type*
 *
 * Parameters:
 *  <struct ZenitType> *source_type: Original type object
 *  <struct ZenitType> *target_cast_type: Destination type object
 *
 * Returns:
 *  <bool>: *true* if the source type can be casted to the target type, otherwise, *false*.
 * 
 */
bool zenit_type_is_castable_to(struct ZenitType *source_type, struct ZenitType *target_cast_type);

/*
 * Function: zenit_type_fre
 *  Frees the memory allocated in the type object
 *
 * Parameters:
 *  <struct ZenitType> *type: Type object to be freed
 *
 * Returns:
 *  void: This function does not return a value
 *
 * Notes:
 *  This function accepts any "descendant" of the "base" type information struct
 */
void zenit_type_free(struct ZenitType *type);

#endif /* ZENIT_TYPE_H */
