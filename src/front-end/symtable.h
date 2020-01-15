#ifndef ZENIT_SYMTABLE_H
#define ZENIT_SYMTABLE_H

#include <fllib.h>
#include "symbol.h"

/*
 * Struct: struct ZenitSymtable
 *  A symbol table object that keeps track of the program's symbols
 * 
 * Members:
 *  <const char> *id: Symbol table identifier
 *  <FlHashtable> symbols: Hashtable of symbols using the name (string) as key
 *  <enum ZenitScopeType> type: The type of symbol table
 * 
 */
struct ZenitSymtable {
    FlHashtable symbols;
    FlList names;
};

/*
 * Function: zenit_symtable_new
 *  Creates a new symbol table
 *
 * Parameters:
 *  <enum ZenitScopeType> type: The type of symbol table
 *  <const char> *id: An identifier for the symbol table
 *
 * Returns:
 *  <struct ZenitSymtable>: The created symbol table
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_symtable_free> function
 */
struct ZenitSymtable zenit_symtable_new(void);

/*
 * Function: zenit_symtable_free
 *  Releases the memory allocated in the *symtable* object
 *
 * Parameters:
 *  <struct ZenitSymtable> *symtable: Symbol table to be freed
 *
 * Returns:
 *  void: This function does not return a value
 * 
 */
void zenit_symtable_free(struct ZenitSymtable *symtable);

/*
 * Function: zenit_symtable_add
 *  Adds the <struct ZenitSymbol> object to the symbol table
 *
 * Parameters:
 *  <struct ZenitSymtable> *symtable: The symbol table
 *  <struct ZenitSymbol> *symbol: The symbol to be added to the symbol table
 *
 * Returns:
 *  <struct ZenitSymbol>*: The symbol object
 *
 */
struct ZenitSymbol* zenit_symtable_add(struct ZenitSymtable *symtable, struct ZenitSymbol *symbol);

/*
 * Function: zenit_symtable_has
 *  Checks if a symbol exists in the symbol table
 *
 * Parameters:
 *  <struct ZenitSymtable> *symtable: The symbol table
 *  <const char> *symbol_name: The symbol's name to look for
 *
 * Returns:
 *  bool: *true* if there is a symbol with the provided name, otherwise this function
 *          returns *false*.
 *
 */
bool zenit_symtable_has(struct ZenitSymtable *symtable, const char *symbol_name);

/*
 * Function: zenit_symtable_get
 *  This function returns a symbol from the symbol table if there is a value for the provided *symbol_name* key
 *
 * Parameters:
 *  <struct ZenitSymtable> *symtable: Symbol table
 *  <const char> *symbol_name: Key to lookup the symbol
 *
 * Returns:
 *  <struct ZenitSymbol>*: Pointer to the symbol object or NULL
 *
 */
struct ZenitSymbol* zenit_symtable_get(struct ZenitSymtable *symtable, const char *symbol_name);

/*
 * Function: zenit_symtable_remove
 *  This function removes the symbol that matches with the *symbol_name* from the symbol table
 *  and returns the removed object
 *
 * Parameters:
 *  <struct ZenitSymtable> *symtable: Symbol table
 *  <const char> *symbol_name: Key to lookup the symbol to remove
 *
 * Returns:
 *  struct ZenitSymbol*: Removed symbol
 *
 */
struct ZenitSymbol* zenit_symtable_remove(struct ZenitSymtable *symtable, const char *symbol_name);

bool zenit_symtable_is_empty(struct ZenitSymtable *symtable);

char* zenit_symtable_dump(struct ZenitSymtable *symtable, char *output, bool verbose);

#endif /* ZENIT_SYMTABLE_H */
