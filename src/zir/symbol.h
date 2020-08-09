#ifndef ZIR_SYMBOL_H
#define ZIR_SYMBOL_H

#include <stdbool.h>
#include "types/system.h"

/*
 * Struct: ZirSymbol
 *  Represents a symbol of a ZIR program containing the identifier name and
 *  the type information.
 */
typedef struct ZirSymbol {
    const char *name;
    ZirType *type;
} ZirSymbol;

/*
 * Function: zir_symbol_new
 *  Allocates memory for a new symbol object and initializes it with the provided
 *  identifier name and type information
 *
 * Parameters:
 *  name - Symbol name
 *  type - Type information
 *  temporal - True if the symbol is a temporal object
 *
 * Returns:
 *  ZirSymbol* - The new symbol
 * 
 * Notes:
 *  The object returned by this function must be freed with the <zir_symbol_free> function
 *
 */
ZirSymbol* zir_symbol_new(const char *name, ZirType *type);

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
void zir_symbol_free(ZirSymbol *symbol);

/*
 * Function: zir_symbol_dump
 *  Appends a dump of the symbol object to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZirSymbol> *symbol: Symbol object to dump to the output
 *  <char> *output: Pointer to a heap allocated string
 *
 * Returns:
 *  char*: Pointer to the output string
 *
 * Notes:
 *  Because the *output* pointer can be modified, this function returns
 *  a pointer to the new location in case the memory is reallocated or
 *  to the old location in case the pointer does not need to be modified. Either
 *  way, it is safe to use the function as:
 *      output = zir_symbol_dump(symbol, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zir_symbol_dump(ZirSymbol *symbol, char *output);

#endif /* ZIR_SYMBOL_H */
