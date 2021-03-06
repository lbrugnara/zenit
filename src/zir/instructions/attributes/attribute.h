#ifndef ZIR_ATTRIBUTE_H
#define ZIR_ATTRIBUTE_H

#include "property-map.h"

/*
 * Struct: ZirAttribute
 *  Represents an instruction's attribute
 * 
 * Members:
 *  <char> *name: The attribute name
 *  <ZirPropertyMap> *properties: The set of properties associated with this attribute
 */
typedef struct ZirAttribute {
    char *name;
    ZirPropertyMap *properties;
} ZirAttribute;

/*
 * Function: zir_attribute_new
 *  Crates a new attribute with the provided *name*
 *
 * Parameters:
 *  <char> *name: The attribute's name
 *
 * Returns:
 *  ZirAttribute*: The attribute object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_attribute_free> function
 */
ZirAttribute* zir_attribute_new(char *name);

/*
 * Function: zir_attribute_free
 *  Releases the memory of the attribute, including its properties
 *
 * Parameters:
 *  <ZirAttribute> *attribute: The attribute object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_attribute_free(ZirAttribute *attribute);

#endif /* ZIR_ATTRIBUTE_H */
