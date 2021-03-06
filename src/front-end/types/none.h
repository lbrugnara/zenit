#ifndef ZENIT_TYPE_NONE_H
#define ZENIT_TYPE_NONE_H

#include "type.h"

/*
 * Function: zenit_none_type_new
 *  Returns a new instance of a type that represents the lack of type
 *
 * Parameters:
 *  void: this function does not take parameters
 *
 * Returns:
 *  ZenitType*: Pointer to a a none type object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_none_type_free> function
 */
ZenitType* zenit_none_type_new(void);

/*
 * Function: zenit_none_type_hash
 *  Returns a hash that identifies the none type object
 *
 * Parameters:
 *  <ZenitType> *type: Type object
 *
 * Returns:
 *  unsigned long: Hash code of the type object
 */
unsigned long zenit_none_type_hash(ZenitType *type);

/*
 * Function: zenit_none_type_free
 *  Frees the memory of the none type object
 *
 * Parameters:
 *  <ZenitType> *type: Type object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_none_type_free(ZenitType *type);

#endif /* ZENIT_TYPE_NONE_H */
