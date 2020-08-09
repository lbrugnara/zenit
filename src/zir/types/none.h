#ifndef ZIR_TYPE_NONE_H
#define ZIR_TYPE_NONE_H

#include "type.h"

/*
 * Function: zir_type_none_new
 *  Returns a new instance of a type that represents the lack of type
 *
 * Parameters:
 *  void: this function does not take parameters
 *
 * Returns:
 *  ZirType*: Pointer to a a none type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_type_none_free> function
 */
ZirType* zir_type_none_new(void);

/*
 * Function: zir_type_none_hash
 *  Returns a hash that identifies the none type object
 *
 * Parameters:
 *  <ZirType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zir_type_none_hash(ZirType *type);

/*
 * Function: zir_type_none_free
 *  Frees the memory of the none type object
 *
 * Parameters:
 *  <ZirType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_type_none_free(ZirType *type);

#endif /* ZIR_TYPE_NONE_H */
