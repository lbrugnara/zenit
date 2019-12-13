#ifndef ZIR_SYMBOL_H
#define ZIR_SYMBOL_H

#include <stdbool.h>
#include "types/system.h"

/*
 * Struct: struct ZirSymbol
 *  Represents a symbol of a ZIR program containing the identifier name and
 *  the type information.
 */
struct ZirSymbol {
    const char *name;
    struct ZirTypeInfo *typeinfo;
};

/*
 * Function: zir_symbol_new
 *  Allocates memory for a new symbol object and initializes it with the provided
 *  identifier name and type information
 *
 * Parameters:
 *  name - Symbol name
 *  typeinfo - Type information
 *  temporal - True if the symbol is a temporal object
 *
 * Returns:
 *  struct ZirSymbol* - The new symbol
 * 
 * Notes:
 *  The object returned by this function must be freed with the <zir_symbol_free> function
 *
 */
struct ZirSymbol* zir_symbol_new(const char *name, struct ZirTypeInfo *typeinfo);

/*
 * Function: zir_symbol_free
 *  Releases the memory of the *symbol* object
 *
 * Parameters:
 *  symbol - Object to be freed
 *
 * Returns:
 *  void - This function does not return a value
 */
void zir_symbol_free(struct ZirSymbol *symbol);

#endif /* ZIR_SYMBOL_H */
