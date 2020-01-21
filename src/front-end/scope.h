#ifndef ZENIT_SCOPE_H
#define ZENIT_SCOPE_H

#include "symtable.h"

/*
 * Enum: enum ZenitScopeType
 *  Represents the different type of scopes
 * 
 */
enum ZenitScopeType {
    ZENIT_SCOPE_GLOBAL,
    ZENIT_SCOPE_STRUCT,
    ZENIT_SCOPE_FUNCTION,
};


/*
 * Struct: zenit_scope_new
 *  Represents a scope in the program
 * 
 * Members:
 *  <struct ZenitScope> *parent: Pointer to the parent scope
 *  <struct ZenitScope> **children: Array of pointers to the scope's children
 *  <struct ZenitSymtable> symtable: Symbol table of the current scope
 *  <unsigned long long> temp_counter: Counter for temporal symbols names
 * 
 */
struct ZenitScope {
    const char *id;
    struct ZenitScope *parent;
    struct ZenitScope **children;
    unsigned long long temp_counter;
    struct ZenitSymtable symtable;
    enum ZenitScopeType type;
};

/*
 * Function: zenit_scope_new
 *  Creates a new scope object
 *
 * Parameters:
 *  <const char> *id: Id of the scope object
 *  <enum ZenitScopeType> type: Type of symbol table for this scope
 *  <struct ZenitScope> *parent: Pointer to a parent scope
 *
 * Returns:
 *  <struct ZenitScope>*: The created scope object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_scope_free> function
 */
struct ZenitScope* zenit_scope_new(const char *id, enum ZenitScopeType type, struct ZenitScope *parent);

/*
 * Function: zenit_scope_free
 *  Releases the memory of the scope object
 *
 * Parameters:
 *  <struct ZenitScope> *scope: Scope to be freed
 *
 * Returns:
 *  <void>: This function does not return a value
 * 
 */
void zenit_scope_free(struct ZenitScope *scope);

/*
 * Function: zenit_scope_add_symbol
 *  Adds a new symbol to the scope object
 *
 * Parameters:
 *  <struct ZenitScope> *scope: Scope object
 *  <struct ZenitSymbol> *symbol: Target symbol to be added to the scope
 *
 * Returns:
 *  struct ZenitSymbol*: Pointer to the added symbol 
 *
 * Notes:
 *  The scope object takes ownership of the symbol, which means that the memory of the symbol
 *  object will be freed when the scope is freed.
 */
struct ZenitSymbol* zenit_scope_add_symbol(struct ZenitScope *scope, struct ZenitSymbol *symbol);

/*
 * Function: zenit_scope_has_symbol
 *  Returns *true* or *false* based on the existence of a symbol with its name equals to the *symbol_name*
 *  variable within the current scope.
 *
 * Parameters:
 *  <struct ZenitScope> *scope: Scope object
 *  <const char> *symbol_name: Name of the target symbol
 *
 * Returns:
 *  bool: *true* if a symbol with the requested name exists within the scope, otherwise *false*.
 */
bool zenit_scope_has_symbol(struct ZenitScope *scope, const char *symbol_name);

/*
 * Function: zenit_scope_get_symbol
 *  Returns (if it exists) the symbol with name equals to *symbol_name* within the current scope
 *
 * Parameters:
 *  <struct ZenitScope> *scope: Scope object
 *  <const char> *symbol_name: Name of the target symbol
 *
 * Returns:
 *  struct ZenitSymbol*: Symbol matching with the provided name, otherwise <NULL>
 */
struct ZenitSymbol* zenit_scope_get_symbol(struct ZenitScope *scope, const char *symbol_name);

/*
 * Function: zenit_program_remove_symbol
 *  Removes from the scope -if it exists- a symbol with the name equals to the string *symbol_name*
 *
 * Parameters:
 *  <struct ZenitScope> *scope: Scope object
 *  <const char> *symbol_name: Name of the symbol to remove
 *
 * Returns:
 *  struct ZenitSymbol*: Symbol removed from the scope if it exists, otherwise this function returns <NULL>
 *
 * Notes:
 *  The scope loses ownership of the symbol object, which means that the caller is in charge of freeing the
 *  symbol's memory.
 */
struct ZenitSymbol* zenit_scope_remove_symbol(struct ZenitScope *scope, const char *symbol_name);

/*
 * Function: zenit_scope_get_symbols
 *  Returns an array containing all the symbols in the current scope.
 *
 * Parameters:
 *  <struct ZenitScope> *scope: Scope object
 *  <bool> include_temporals: If *true*, the temporal symbols within the scope are added to the output array. 
 *
 * Returns:
 *  struct ZenitSymbol**:  Array to a pointer of <struct ZenitSymbol> objects
 *
 * Notes:
 *  The array returned by this function must be freed with the <fl_array_free> function
 */
struct ZenitSymbol** zenit_scope_get_symbols(struct ZenitScope *scope, bool include_temporals);

/*
 * Function: zenit_scope_has_symbols
 *  Returns *true* or *false* based on if the scope has symbols or not
 *
 * Parameters:
 *  <struct ZenitScope> *scope: Scope object
 *
 * Returns:
 *  bool: *true* if the scope contains symbols, otherwise it returns *false*
 *
 * Notes:
 *  This function does take into account the temporal symbols too
 */
bool zenit_scope_has_symbols(struct ZenitScope *scope);

/*
 * Function: zenit_scope_dump
 *  Appends a dump of the scope object to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitScope> *scope: Scope object to dump to the output
 *  <char> *output: Pointer to a heap allocated string
 *  <bool> verbose: If true, the temporal symbols are added to the output, otherwise they are ignored
 *
 * Returns:
 *  char*: Pointer to the output string
 *
 * Notes:
 *  Because the *output* pointer can be modified, this function returns
 *  a pointer to the new location in case the memory is reallocated or
 *  to the old one in case the pointer does not need to be modified. Either
 *  way, it is safe to use the function as:
 *      output = zenit_scope_dump(scope, output, verbose);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_scope_dump(struct ZenitScope *scope, char *output, bool verbose);

#endif /* ZENIT_SCOPE_H */
