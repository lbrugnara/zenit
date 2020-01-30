#ifndef ZIR_ATTRIBUTE_H
#define ZIR_ATTRIBUTE_H

#include "property_map.h"

/*
 * Struct: struct ZirAttribute
 *  Represents an instruction's attribute
 * 
 * Members:
 *  <char> *name: The attribute name
 *  <struct ZirPropertyMap> properties: The set of properties associated with this attribute
 */
struct ZirAttribute {
    char *name;
    struct ZirPropertyMap properties;
};

/*
 * Function: zir_attribute_new
 *  Crates a new attribute with the provided *name*
 *
 * Parameters:
 *  <char> *name: The attribute's name
 *
 * Returns:
 *  struct ZirAttribute*: The attribute object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_attribute_free> function
 */
struct ZirAttribute* zir_attribute_new(char *name);

/*
 * Function: zir_attribute_free
 *  Releases the memory of the attribute, including its properties
 *
 * Parameters:
 *  <struct ZirAttribute> *attribute: The attribute object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_attribute_free(struct ZirAttribute *attribute);

#endif /* ZIR_ATTRIBUTE_H */
