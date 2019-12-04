#ifndef ZENIT_IR_SYMBOL_H
#define ZENIT_IR_SYMBOL_H

#include <stdbool.h>
#include "type.h"

/*
 * Struct: struct ZenitIrSymbol
 *  Represents a symbol of a ZIR program containing the identifier name and
 *  the type information.
 */
struct ZenitIrSymbol {
    const char *name;
    struct ZenitIrTypeInfo typeinfo;
    bool temporal;
};

/*
 * Function: zenit_ir_symbol_new
 *  Allocates memory for a new symbol object and initializes it with the provided
 *  identifier name and type information
 *
 * Parameters:
 *  name - Symbol name
 *  typeinfo - Type information
 *  temporal - True if the symbol is a temporal object
 *
 * Returns:
 *  struct ZenitIrSymbol* - The new symbol
 * 
 * Notes:
 *  The object returned by this function must be freed with the <zenit_ir_symbol_free> function
 *
 */
struct ZenitIrSymbol* zenit_ir_symbol_new(const char *name, struct ZenitIrTypeInfo *typeinfo, bool temporal);

/*
 * Function: zenit_ir_symbol_free
 *  Releases the memory of the *symbol* object
 *
 * Parameters:
 *  symbol - Object to be freed
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_ir_symbol_free(struct ZenitIrSymbol *symbol);

#endif /* ZENIT_IR_SYMBOL_H */
