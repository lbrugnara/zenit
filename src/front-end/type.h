#ifndef ZENIT_TYPE_H
#define ZENIT_TYPE_H

/*
 * Enum: enum ZenitType
 *  Enumerates all the native type supported by Zenit
 *  including some special values that the compiler
 *  uses internally
 *
 */
enum ZenitType {
    ZENIT_TYPE_NONE,
    ZENIT_TYPE_CUSTOM,

    ZENIT_TYPE_UINT8,
    ZENIT_TYPE_UINT16,

    ZENIT_TYPE_END
};

/*
 * Struct: struct ZenitTypeInfo
 *  Represents the information of a type. 
 * 
 * Members:
 *  <const char> *name: If the <enum ZenitType> is <ZENIT_TYPE_CUSTOM> this property should be present, otherwise NULL.
 *  <enum ZenitType> type: The raw type.
 *  <size_t> elements: Number of elements (1 for simple variables. 0, 1 or greater for arrays)
 *  <bool> is_array: True if the variable is an array
 *  <bool> is_ref: True if the variable is a reference to a variable
 *  
 */
struct ZenitTypeInfo {
    const char *name;
    enum ZenitType type;
    size_t elements;
    bool is_array;
    bool is_ref;
};

/*
 * Function: zenit_type_string_parse
 *  Takes a string and returns the system type for that
 *  representation.
 *
 * Parameters:
 *  typestr - String to match against the different system types
 *
 * Returns:
 *  enum ZenitType - The type that matches the string
 *
 */
enum ZenitType zenit_type_string_parse(const char *typestr);


/*
 * Function: zenit_type_slice_parse
 *  This function is similar to <zenit_type_string_parse>, the only
 *  difference is it takes a <struct FlSlice> object
 *
 * Parameters:
 *  slice - Sequence of bytes to match against the different system types
 *
 * Returns:
 *  enum ZenitType - The type that matches the sequence of bytes
 *
 */
enum ZenitType zenit_type_slice_parse(struct FlSlice *slice);

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
 *  const char* - String representation of the *typeinfo* object
 * 
 * Notes:
 *  To get the base type without any other
 *  information refer to the <zenit_type_to_base_string> function
 *
 */
const char* zenit_type_to_string(const struct ZenitTypeInfo *typeinfo);

/*
 * Function: zenit_type_to_base_string
 *  Returns the string representation of the <enum ZenitType> within the *typeinfo*
 *  object without any information regarding if it is an array or a reference.
 *
 * Parameters:
 *  typeinfo - <struct ZenitTypeInfo> object to get its <enum ZenitType> string representation.
 *
 * Returns:
 *  const char* - String representation of the <enum ZenitType> within the *typeinfo* object
 *
 */
const char* zenit_type_to_base_string(const struct ZenitTypeInfo *typeinfo);

/*
 * Function: zenit_type_copy
 *  Copies the information from the *src_type* into the *dest_type*
 *
 * Parameters:
 *  dest_type - Destination type object
 *  src_type - Source type object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_type_copy(struct ZenitTypeInfo *dest_type, struct ZenitTypeInfo *src_type);

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

bool zenit_type_unify(struct ZenitTypeInfo *type_a, struct ZenitTypeInfo *type_b);

bool zenit_type_can_assign(struct ZenitTypeInfo *target_type, struct ZenitTypeInfo *value_type);

bool zenit_type_can_cast(struct ZenitTypeInfo *target_type, struct ZenitTypeInfo *cast_type);

void zenit_type_free(struct ZenitTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_H */
