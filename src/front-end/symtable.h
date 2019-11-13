#ifndef ZENIT_SYMTABLE_H
#define ZENIT_SYMTABLE_H

#include <fllib.h>
#include "symbol.h"

/*
 * Enum: enum ZenitSymbolTableType
 *  Represents the different type of symbol tables
 * 
 */
enum ZenitSymbolTableType {
    ZENIT_SYMTABLE_GLOBAL,
    ZENIT_SYMTABLE_FUNCTION,
};

/*
 * Struct: struct ZenitSymbolTable
 *  A symbol table object that keeps track of the program's symbols
 * 
 * Members:
 *  <const char> *id: Symbol table identifier
 *  <FlHashtable> symbols: Hashtable of symbols using the name (string) as key
 *  <enum ZenitSymbolTableType> type: The type of symbol table
 * 
 */
struct ZenitSymbolTable {
    const char *id;
    FlHashtable symbols;
    enum ZenitSymbolTableType type;
};

/*
 * Function: zenit_symtable_new
 *  Creates a new symbol table
 *
 * Parameters:
 *  <enum ZenitSymbolTableType> type - The type of symbol table
 *  <const char> *id: An identifier for the symbol table
 *
 * Returns:
 *  <struct ZenitSymbolTable> - The created symbol table
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_symtable_free> function
 */
struct ZenitSymbolTable zenit_symtable_new(enum ZenitSymbolTableType, const char *id);

/*
 * Function: zenit_symtable_free
 *  Releases the memory allocated in the *symtable* object
 *
 * Parameters:
 *  <struct ZenitSymbolTable> *symtable - Symbol table to be freed
 *
 * Returns:
 *  void - This function does not return a value
 * 
 */
void zenit_symtable_free(struct ZenitSymbolTable *symtable);

/*
 * Function: zenit_symtable_add
 *  Adds the <struct ZenitSymbol> object to the symbol table
 *
 * Parameters:
 *  <struct ZenitSymbolTable> *symtable - The symbol table
 *  <struct ZenitSymbol> *symbol - The symbol to be added to the symbol table
 *
 * Returns:
 *  <struct ZenitSymbol>* - The symbol object
 *
 */
struct ZenitSymbol* zenit_symtable_add(struct ZenitSymbolTable *symtable, struct ZenitSymbol *symbol);

/*
 * Function: zenit_symtable_has
 *  Checks if a symbol exists in the symbol table
 *
 * Parameters:
 *  <struct ZenitSymbolTable> *symtable - The symbol table
 *  <const char> *symbol_name - The symbol's name to look for
 *
 * Returns:
 *  bool - *true* if there is a symbol with the provided name, otherwise this function
 *          returns *false*.
 *
 */
bool zenit_symtable_has(struct ZenitSymbolTable *symtable, const char *symbol_name);

/*
 * Function: zenit_symtable_get
 *  This function returns a symbol from the symbol table if there is a value for the provided *symbol_name* key
 *
 * Parameters:
 *  <struct ZenitSymbolTable> *symtable - Symbol table
 *  <const char> *symbol_name - Key to lookup the symbol
 *
 * Returns:
 *  <struct ZenitSymbol>* - Pointer to the symbol object or NULL
 *
 */
struct ZenitSymbol* zenit_symtable_get(struct ZenitSymbolTable *symtable, const char *symbol_name);

#endif /* ZENIT_SYMTABLE_H */
