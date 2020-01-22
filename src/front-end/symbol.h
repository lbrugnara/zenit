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
    struct ZenitType *type;
};

/*
 * Function: zenit_symbol_new
 *  Allocates memory for a new symbol object and initializes it with the provided
 *  identifier name and type information
 *
 * Parameters:
 *  name - Symbol name
 *  type - Type information
 *
 * Returns:
 *  struct ZenitSymbol* - The new symbol
 * 
 * Notes:
 *  The object returned by this function must be freed with the <zenit_symbol_free> function
 *
 */
struct ZenitSymbol* zenit_symbol_new(const char *name, struct ZenitType *type);

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

/*
 * Function: zenit_symbol_dump
 *  Appends a dump of the symbol object to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitSymbol> *symbol: Symbol object to dump to the output
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
 *      output = zenit_symbol_dump(symbol, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_symbol_dump(struct ZenitSymbol *symbol, char *output);

#endif /* ZENIT_SYMBOL_H */
