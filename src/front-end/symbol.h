#ifndef ZENIT_SYMBOL_H
#define ZENIT_SYMBOL_H

#include <fllib.h>
#include "types/type.h"

/*
 * Struct: struct ZenitSymbol
 *  Represents a symbol of the source program containing an identifier name
 *  and the type information.
 */
struct ZenitSymbol {
    const char *name;
    struct ZenitTypeInfo *typeinfo;
};

/*
 * Function: zenit_symbol_new
 *  Allocates memory for a new symbol object and initializes it with the provided
 *  identifier name and type information
 *
 * Parameters:
 *  name - Symbol name
 *  typeinfo - Type information
 *
 * Returns:
 *  struct ZenitSymbol* - The new symbol
 * 
 * Notes:
 *  The object returned by this function must be freed with the <zenit_symbol_free> function
 *
 */
struct ZenitSymbol* zenit_symbol_new(const char *name, struct ZenitTypeInfo *typeinfo);

/*
 * Function: zenit_symbol_free
 *  Releases the memory of the *symbol* object
 *
 * Parameters:
 *  symbol - Object to be freed
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_symbol_free(struct ZenitSymbol *symbol);

#endif /* ZENIT_SYMBOL_H */
