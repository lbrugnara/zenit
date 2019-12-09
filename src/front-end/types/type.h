#ifndef ZENIT_TYPE_H
#define ZENIT_TYPE_H

#define zenit_type_is_primitive(type) (type >= ZENIT_TYPE_UINT8 && type <= ZENIT_TYPE_UINT16)

/*
 * Enum: enum ZenitType
 *  Enumerates all the native type supported by Zenit
 *  including some special values that the compiler
 *  uses internally
 *
 */
enum ZenitType {
    ZENIT_TYPE_NONE,

    ZENIT_TYPE_REFERENCE,

    ZENIT_TYPE_ARRAY,

    ZENIT_TYPE_STRUCT,

    ZENIT_TYPE_UINT8,
    ZENIT_TYPE_UINT16,

    ZENIT_TYPE_END
};

struct ZenitTypeString {
    unsigned long version;
    char *value;
};

/*
 * Struct: struct ZenitTypeInfo
 *  Represents the information of a type. 
 * 
 * Members:
 *  <const char> *name: If the <enum ZenitType> is <ZENIT_TYPE_STRUCT> this property should be present, otherwise NULL.
 *  <enum ZenitType> type: The raw type.
 *  <size_t> elements: Number of elements (1 for simple variables. 0, 1 or greater for arrays)
 *  <bool> is_array: True if the variable is an array
 *  <bool> is_ref: True if the variable is a reference to a variable
 *  
 */
struct ZenitTypeInfo {
    enum ZenitType type;
    struct ZenitTypeString to_string;
};

unsigned long zenit_type_hash(struct ZenitTypeInfo *typeinfo);

/*
 * Function: zenit_type_from_slice
 *  Takes a string slice and returns the system type for that
 *  representation.
 *
 * Parameters:
 *  slice - Sequence of bytes to match against the different system types
 *
 * Returns:
 *  enum ZenitType - The type that matches the sequence of bytes
 *
 */
enum ZenitType zenit_type_from_slice(struct FlSlice *slice);

/*
 * Function: zenit_type_to_string
 *  Returns a string representation of the <struct ZenitTypeInfo> object.
 *  If the type is an array type, the string representation contains
 *  the size information. If the the type is a reference it also contains that
 *  information.
 *
 * Parameters:
 *  typeinfo - <struct ZenitTypeInfo> object to get its string representation.
 *
 * Returns:
 *  char* - String representation of the *typeinfo* object
 *
 */
char* zenit_type_to_string(struct ZenitTypeInfo *typeinfo);

/*
 * Function: zenit_type_equals
 *  Compares *type_a* and *type_b* to know if they are equals, including
 *  if they are arrays and their elements count
 *
 * Parameters:
 *  type_a - Object to compare
 *  type_b - Object to compare
 *
 * Returns:
 *  bool - *true* if the objects are equals, otherwise *false*.
 *
 */
bool zenit_type_equals(struct ZenitTypeInfo *type_a, struct ZenitTypeInfo *type_b);

struct ZenitTypeInfo* zenit_type_copy(struct ZenitTypeInfo *src_type);

struct ZenitTypeInfo* zenit_type_unify(struct ZenitTypeInfo *type_a, struct ZenitTypeInfo *type_b);

bool zenit_type_is_assignable_from(struct ZenitTypeInfo *target_type, struct ZenitTypeInfo *value_type);

bool zenit_type_is_castable_to(struct ZenitTypeInfo *source_type, struct ZenitTypeInfo *target_cast_type);

void zenit_type_free(struct ZenitTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_H */
