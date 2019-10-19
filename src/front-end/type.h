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
 * Type: CenitTypeInfo
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
 * Function: cenit_type_parse
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
CenitType cenit_type_parse(const char *typestr);


/*
 * Function: cenit_type_parse_slice
 *  This function is similar to <cenit_type_parse>, the only
 *  difference is it takes a <struct FlSlice> object
 *
 * Parameters:
 *  slice - Sequence of bytes to match against the different system types
 *
 * Returns:
 *  CenitType - The type that matches the sequence of bytes
 *
 */
CenitType cenit_type_parse_slice(struct FlSlice *slice);

/*
 * Function: cenit_type_string
 *  Returns an string representation of the <CenitType> *type*
 *
 * Parameters:
 *  type - Type
 *
 * Returns:
 *  const char* - String representation of the *type*
 *
 */
const char* cenit_type_string(CenitType type);

void cenit_type_copy(CenitTypeInfo *dest_type, CenitTypeInfo *src_type);

bool cenit_type_equals(CenitTypeInfo *type_a, CenitTypeInfo *type_b);

#endif /* CENIT_TYPE_H */
