#ifndef ZIR_SYMTABLE_H
#define ZIR_SYMTABLE_H

#include <fllib.h>
#include "symbol.h"

/*
 * Enum: enum ZirSymbolTableType
 *  Represents the different type of symbol tables
 * 
 */
enum ZirSymbolTableType {
    ZIR_SYMTABLE_GLOBAL,
    ZIR_SYMTABLE_FUNCTION,
};

/*
 * Struct: struct ZirSymbolTable
 *  A symbol table object that keeps track of the program's symbols
 * 
 * Members:
 *  <const char> *id: Symbol table identifier
 *  <FlHashtable> symbols: Hashtable of symbols using the name (string) as key
 *  <enum ZirSymbolTableType> type: The type of symbol table
 * 
 */
struct ZirSymbolTable {
    const char *id;
    FlHashtable symbols;
    enum ZirSymbolTableType type;
};

/*
 * Function: zir_symtable_new
 *  Creates a new symbol table
 *
 * Parameters:
 *  <enum ZirSymbolTableType> type - The type of symbol table
 *  <const char> *id: An identifier for the symbol table
 *
 * Returns:
 *  <struct ZirSymbolTable> - The created symbol table
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_symtable_free> function
 */
struct ZirSymbolTable zir_symtable_new(enum ZirSymbolTableType, const char *id);

/*
 * Function: zir_symtable_free
 *  Releases the memory allocated in the *symtable* object
 *
 * Parameters:
 *  <struct ZirSymbolTable> *symtable - Symbol table to be freed
 *
 * Returns:
 *  void - This function does not return a value
 * 
 */
void zir_symtable_free(struct ZirSymbolTable *symtable);

/*
 * Function: zir_symtable_add
 *  Adds the <struct ZirSymbol> object to the symbol table
 *
 * Parameters:
 *  <struct ZirSymbolTable> *symtable - The symbol table
 *  <struct ZirSymbol> *symbol - The symbol to be added to the symbol table
 *
 * Returns:
 *  <struct ZirSymbol>* - The symbol object
 *
 */
struct ZirSymbol* zir_symtable_add(struct ZirSymbolTable *symtable, struct ZirSymbol *symbol);

/*
 * Function: zir_symtable_has
 *  Checks if a symbol exists in the symbol table
 *
 * Parameters:
 *  <struct ZirSymbolTable> *symtable - The symbol table
 *  <const char> *symbol_name - The symbol's name to look for
 *
 * Returns:
 *  bool - *true* if there is a symbol with the provided name, otherwise this function
 *          returns *false*.
 *
 */
bool zir_symtable_has(struct ZirSymbolTable *symtable, const char *symbol_name);

/*
 * Function: zir_symtable_get
 *  This function returns a symbol from the symbol table if there is a value for the provided *symbol_name* key
 *
 * Parameters:
 *  <struct ZirSymbolTable> *symtable - Symbol table
 *  <const char> *symbol_name - Key to lookup the symbol
 *
 * Returns:
 *  <struct ZirSymbol>* - Pointer to the symbol object or NULL
 *
 */
struct ZirSymbol* zir_symtable_get(struct ZirSymbolTable *symtable, const char *symbol_name);

#endif /* ZIR_SYMTABLE_H */
