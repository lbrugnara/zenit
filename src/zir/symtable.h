#ifndef ZIR_SYMTABLE_H
#define ZIR_SYMTABLE_H


#include <fllib/containers/List.h>
#include <fllib/containers/Hashtable.h>
#include "symbol.h"

/*
 * Struct: struct ZirSymtable
 *  A symbol table object that keeps track of the program's symbols
 * 
 * Members:
 *  <FlHashtable> symbols: Hashtable of symbols using the name (string) as the key
 *  <FlList> *names: List that keeps track of the insertion order of the symbols
 */
struct ZirSymtable {
    FlHashtable *symbols;
    FlList *names;
};

/*
 * Function: zir_symtable_new
 *  Creates a new symbol table
 *
 * Parameters:
 *  void: This function does not take parameters
 *
 * Returns:
 *  <struct ZirSymtable>: The created symbol table
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_symtable_free> function
 */
struct ZirSymtable zir_symtable_new(void);

/*
 * Function: zir_symtable_free
 *  Releases the memory allocated in the *symtable* object
 *
 * Parameters:
 *  <struct ZirSymtable> *symtable: Symbol table to be freed
 *
 * Returns:
 *  void: This function does not return a value
 * 
 * Notes:
 *  The memory of the symbols within the symbol table is released too
 */
void zir_symtable_free(struct ZirSymtable *symtable);

/*
 * Function: zir_symtable_add
 *  Adds the <struct ZirSymbol> object to the symbol table
 *
 * Parameters:
 *  <struct ZirSymtable> *symtable: The symbol table
 *  <struct ZirSymbol> *symbol: The symbol to be added to the symbol table
 *
 * Returns:
 *  <struct ZirSymbol>*: The symbol object
 *
 * Notes:
 *  The symbol table takes ownership of the added symbol
 */
struct ZirSymbol* zir_symtable_add(struct ZirSymtable *symtable, struct ZirSymbol *symbol);

/*
 * Function: zir_symtable_has
 *  Checks if a symbol exists in the symbol table
 *
 * Parameters:
 *  <struct ZirSymtable> *symtable: The symbol table
 *  <const char> *symbol_name: The symbol's name to look for
 *
 * Returns:
 *  bool: *true* if there is a symbol with the provided name, otherwise this function
 *          returns *false*.
 */
bool zir_symtable_has(struct ZirSymtable *symtable, const char *symbol_name);

/*
 * Function: zir_symtable_get
 *  This function returns a symbol from the symbol table if there is a value for the provided *symbol_name* key
 *
 * Parameters:
 *  <struct ZirSymtable> *symtable: Symbol table
 *  <const char> *symbol_name: Key to lookup the symbol
 *
 * Returns:
 *  <struct ZirSymbol>*: Pointer to the symbol object or NULL
 */
struct ZirSymbol* zir_symtable_get(struct ZirSymtable *symtable, const char *symbol_name);

/*
 * Function: zir_symtable_get_all
 *  Returns an array of all the symbols within the symbol table in the order they were inserted
 *
 * Parameters:
 *  <struct ZirSymtable> *symtable: Symbol table object
 *
 * Returns:
 *  struct ZirSymbol**: Array of pointers to <struct ZirSymbol> objects
 *
 * Notes:
 *  The array returned by this function must be freed using the <fl_array_free> function
 */
struct ZirSymbol** zir_symtable_get_all(struct ZirSymtable *symtable);

/*
 * Function: zir_symtable_dump
 *  Appends a dump of the symbol table object to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZirSymtable> *symtable: Symbol table object to dump to the output
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
char* zir_symtable_dump(struct ZirSymtable *symtable, char *output);

#endif /* ZIR_SYMTABLE_H */
