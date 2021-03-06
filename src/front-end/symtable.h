#ifndef ZENIT_SYMTABLE_H
#define ZENIT_SYMTABLE_H


#include <fllib/containers/List.h>
#include <fllib/containers/Hashtable.h>
#include "symbol.h"

typedef FlHashtable ZenitStringToSymbolMap;
typedef FlList ZenitStringList;

/*
 * Struct: struct ZenitSymtable
 *  A symbol table object that keeps track of the program's symbols
 * 
 * Members:
 *  <FlHashtable> *symbols: Hashtable of symbols using the name (string) as key
 *  <FlList> *names: List that keeps track of the insertion order of the symbols
 * 
 */
typedef struct ZenitSymtable {
    ZenitStringToSymbolMap *symbols;
    ZenitStringList *names;
} ZenitSymtable;

/*
 * Function: zenit_symtable_new
 *  Creates a new symbol table
 *
 * Parameters:
 *  <void> - This function does not receive parameters
 *
 * Returns:
 *  <ZenitSymtable>: The created symbol table
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_symtable_free> function
 */
ZenitSymtable zenit_symtable_new(void);

/*
 * Function: zenit_symtable_free
 *  Releases the memory allocated in the *symtable* object
 *
 * Parameters:
 *  <ZenitSymtable> *symtable: Symbol table to be freed
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_symtable_free(ZenitSymtable *symtable);

/*
 * Function: zenit_symtable_add
 *  Adds the <ZenitSymbol> object to the symbol table
 *
 * Parameters:
 *  <ZenitSymtable> *symtable: The symbol table
 *  <ZenitSymbol> *symbol: The symbol to be added to the symbol table
 *
 * Returns:
 *  <ZenitSymbol>*: The symbol object
 * 
 * Notes:
 *  The symbol table takes ownership of all the symbols that are added to it, which means the memory
 *  those symbols will be released when the symbol table object is freed with the <zenit_symtable_free>
 *  function
 */
ZenitSymbol* zenit_symtable_add(ZenitSymtable *symtable, ZenitSymbol *symbol);

/*
 * Function: zenit_symtable_has
 *  Checks if a symbol exists in the symbol table
 *
 * Parameters:
 *  <ZenitSymtable> *symtable: The symbol table
 *  <const char> *symbol_name: The symbol's name to look for
 *
 * Returns:
 *  bool: *true* if there is a symbol with the provided name, otherwise this function
 *          returns *false*.
 *
 */
bool zenit_symtable_has(ZenitSymtable *symtable, const char *symbol_name);

/*
 * Function: zenit_symtable_get
 *  This function returns a symbol from the symbol table if there is a value for the provided *symbol_name* key
 *
 * Parameters:
 *  <ZenitSymtable> *symtable: Symbol table
 *  <const char> *symbol_name: Key to lookup the symbol
 *
 * Returns:
 *  <ZenitSymbol>*: Pointer to the symbol object or NULL
 *
 */
ZenitSymbol* zenit_symtable_get(ZenitSymtable *symtable, const char *symbol_name);

/*
 * Function: zenit_symtable_remove
 *  This function removes the symbol that matches with the *symbol_name* from the symbol table
 *  and returns the removed object
 *
 * Parameters:
 *  <ZenitSymtable> *symtable: Symbol table
 *  <const char> *symbol_name: Key to lookup the symbol to remove
 *
 * Returns:
 *  ZenitSymbol*: Removed symbol
 *
 * Notes:
 *  The symbol table losses ownership of the symbol returned by this function, which means that caller
 *  is in charge of releasing the memory of the symbol.
 */
ZenitSymbol* zenit_symtable_remove(ZenitSymtable *symtable, const char *symbol_name);

/*
 * Function: zenit_symtable_get_all
 *  Returns an array of all the symbols within the symbol table in the order they were inserted
 *
 * Parameters:
 *  <ZenitSymtable> *symtable: Symbol table object
 *  <bool> include_temporals: If true, the tempora symbols within the symbol table are added to the output array.
 *
 * Returns:
 *  ZenitSymbol**: Array of pointers to <ZenitSymbol> objects
 *
 * Notes:
 *  The array returned by this function must be freed using the <fl_array_free> function
 */
ZenitSymbol** zenit_symtable_get_all(ZenitSymtable *symtable, bool include_temporals);

/*
 * Function: zenit_symtable_is_empty
 *  Returns true or false based on if the symbol table contains symbols or not.
 *
 * Parameters:
 *  <ZenitSymtable> *symtable: Symbol table object
 *
 * Returns:
 *  bool: *true* if the symbol table does not contain symbols, otherwise *false*.
 *
 * Notes:
 *  This function does take into account temporal symbols too.
 */
bool zenit_symtable_is_empty(ZenitSymtable *symtable);

/*
 * Function: zenit_symtable_dump
 *  Appends a dump of the symbol table object to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitSymtable> *symtable: Symbol table object to dump to the output
 *  <char> *output: Pointer to a heap allocated string
 *  <bool> verbose: If true, the temporal symbols are added to the output, otherwise they are ignored
 *
 * Returns:
 *  char*: Pointer to the output string
 *
 * Notes:
 *  Because the *output* pointer can be modified, this function returns
 *  a pointer to the new location in case the memory is reallocated or
 *  to the old location in case the pointer does not need to be modified. Either
 *  way, it is safe to use the function as:
 *      output = zenit_symtable_dump(symtable, output, verbose);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_symtable_dump(ZenitSymtable *symtable, char *output, bool verbose);

#endif /* ZENIT_SYMTABLE_H */
