#ifndef ZIR_SYMTABLE_H
#define ZIR_SYMTABLE_H


#include <fllib/containers/List.h>
#include <fllib/containers/Hashtable.h>
#include "symbol.h"

/*
 * Struct: ZirSymtable
 *  A symbol table object that keeps track of the program's symbols
 * 
 * Members:
 *  <FlHashtable> symbols: Hashtable of symbols using the name (string) as the key
 *  <FlList> *names: List that keeps track of the insertion order of the symbols
 */
typedef struct ZirSymtable {
    FlHashtable *symbols;
    FlList *names;
} ZirSymtable;

/*
 * Function: zir_symtable_new
 *  Creates a new symbol table
 *
 * Parameters:
 *  void: This function does not take parameters
 *
 * Returns:
 *  <ZirSymtable>: The created symbol table
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_symtable_free> function
 */
ZirSymtable zir_symtable_new(void);

/*
 * Function: zir_symtable_free
 *  Releases the memory allocated in the *symtable* object
 *
 * Parameters:
 *  <ZirSymtable> *symtable: Symbol table to be freed
 *
 * Returns:
 *  void: This function does not return a value
 * 
 * Notes:
 *  The memory of the symbols within the symbol table is released too
 */
void zir_symtable_free(ZirSymtable *symtable);

/*
 * Function: zir_symtable_add
 *  Adds the <ZirSymbol> object to the symbol table
 *
 * Parameters:
 *  <ZirSymtable> *symtable: The symbol table
 *  <ZirSymbol> *symbol: The symbol to be added to the symbol table
 *
 * Returns:
 *  <ZirSymbol>*: The symbol object
 *
 * Notes:
 *  The symbol table takes ownership of the added symbol
 */
ZirSymbol* zir_symtable_add(ZirSymtable *symtable, ZirSymbol *symbol);

/*
 * Function: zir_symtable_has
 *  Checks if a symbol exists in the symbol table
 *
 * Parameters:
 *  <ZirSymtable> *symtable: The symbol table
 *  <const char> *symbol_name: The symbol's name to look for
 *
 * Returns:
 *  bool: *true* if there is a symbol with the provided name, otherwise this function
 *          returns *false*.
 */
bool zir_symtable_has(ZirSymtable *symtable, const char *symbol_name);

/*
 * Function: zir_symtable_get
 *  This function returns a symbol from the symbol table if there is a value for the provided *symbol_name* key
 *
 * Parameters:
 *  <ZirSymtable> *symtable: Symbol table
 *  <const char> *symbol_name: Key to lookup the symbol
 *
 * Returns:
 *  <ZirSymbol>*: Pointer to the symbol object or NULL
 */
ZirSymbol* zir_symtable_get(ZirSymtable *symtable, const char *symbol_name);

/*
 * Function: zir_symtable_get_all
 *  Returns an array of all the symbols within the symbol table in the order they were inserted
 *
 * Parameters:
 *  <ZirSymtable> *symtable: Symbol table object
 *
 * Returns:
 *  ZirSymbol**: Array of pointers to <ZirSymbol> objects
 *
 * Notes:
 *  The array returned by this function must be freed using the <fl_array_free> function
 */
ZirSymbol** zir_symtable_get_all(ZirSymtable *symtable);

/*
 * Function: zir_symtable_dump
 *  Appends a dump of the symbol table object to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZirSymtable> *symtable: Symbol table object to dump to the output
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
 *      output = zir_symtable_dump(symtable, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zir_symtable_dump(ZirSymtable *symtable, char *output);

#endif /* ZIR_SYMTABLE_H */
