#ifndef ZIR_PROPERTY_H
#define ZIR_PROPERTY_H

#include "../operands/operand.h"

/*
 * Struct: struct ZirProperty
 *  Represents an attribute's property and its value which is an operand object
 * 
 * Members:
 *  <char> *name: The property name
 *  <struct ZirOperand> *value: The property's value
 */
struct ZirProperty {
    char *name;
    struct ZirOperand *value;
};

/*
 * Function: zir_property_new
 *  Creates a new property with the provided *name* and *value*
 *
 * Parameters:
 *  <char> *name: The property name
 *  <struct ZirOperand> *value: The property's value
 *
 * Returns:
 *  <struct ZirProperty>*: The property object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_property_free> function
 */
struct ZirProperty* zir_property_new(char *name, struct ZirOperand *value);

/*
 * Function: zir_property_free
 *  Frees the memory of the property object
 *
 * Parameters:
 *  <struct ZirProperty> *property: The property object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_property_free(struct ZirProperty *property);

#endif /* ZIR_PROPERTY_H */
