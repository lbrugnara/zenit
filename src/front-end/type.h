#ifndef CENIT_TYPE_H
#define CENIT_TYPE_H

/*
 * Enum: CenitType
 *  Enumerates all the native type supported by Zenit
 *  including some special values that the compiler
 *  uses internally
 *
 */
typedef enum CenitType {
    CENIT_TYPE_NONE,
    CENIT_TYPE_CUSTOM,
    CENIT_TYPE_UINT8,
} CenitType;

/*
 * Struct: CenitTypeInfo
 *  Represents the information of a system type. The property
 *  *name* is present when the <CenitType> is <CENIT_TYPE_CUSTOM>,
 *  otherwise it is NULL.
 *  
 */
typedef struct CenitTypeInfo {
    size_t elements;
    const char *name;
    CenitType type;
    bool is_array;
} CenitTypeInfo;

/*
 * Function: cenit_type_to_string_parse
 *  Takes a string and returns the system type for that
 *  representation.
 *
 * Parameters:
 *  typestr - String to match against the different system types
 *
 * Returns:
 *  CenitType - The type that matches the string
 *
 */
CenitType cenit_type_to_string_parse(const char *typestr);


/*
 * Function: cenit_type_slice_parse
 *  This function is similar to <cenit_type_to_string_parse>, the only
 *  difference is it takes a <struct FlSlice> object
 *
 * Parameters:
 *  slice - Sequence of bytes to match against the different system types
 *
 * Returns:
 *  CenitType - The type that matches the sequence of bytes
 *
 */
CenitType cenit_type_slice_parse(struct FlSlice *slice);

/*
 * Function: cenit_type_to_string
 *  Returns an string representation of the <CenitType> *typeinfo*.
 *  If the type is an array type, the string representation contains
 *  the size information. To get the base type without any other
 *  information refer to the <cenit_type_to_base_string> function
 *
 * Parameters:
 *  typeinfo - <CenitTypeInfo> object to get its <CenitType>.
 *
 * Returns:
 *  const char* - String representation of the *typeinfo*
 *
 */
const char* cenit_type_to_string(const CenitTypeInfo *typeinfo);

/*
 * Function: cenit_type_to_base_string
 *  Returns the string representation of the <CenitType> *typeinfo*
 *  without any information regarding if it is an array or not
 *  or its size
 *
 * Parameters:
 *  typeinfo - <CenitTypeInfo> object to get its <CenitType>.
 *
 * Returns:
 *  const char* - String representation of the base *typeinfo*
 *
 */
const char* cenit_type_to_base_string(const CenitTypeInfo *typeinfo);

/*
 * Function: cenit_type_copy
 *  Copies the information from the *src_type* into the *dest_type*
 *
 * Parameters:
 *  dest_type - Destination type object
 *  src_type - Source type object
 *
 * Returns:
 *  void - This function does not return a value
 */
void cenit_type_copy(CenitTypeInfo *dest_type, CenitTypeInfo *src_type);

/*
 * Function: cenit_type_equals
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
bool cenit_type_equals(CenitTypeInfo *type_a, CenitTypeInfo *type_b);

#endif /* CENIT_TYPE_H */
