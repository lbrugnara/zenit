#ifndef ZENIT_PROGRAM_H
#define ZENIT_PROGRAM_H

#include "scope.h"

/*
 * Struct: struct ZenitProgram
 *  Represents a Zenit program
 * 
 * Members:
 *  <struct ZenitScope> *global: A pointer to the global scope
 *  <struct ZenitScope> *current: A pointer to the current scope
 */
struct ZenitProgram {
    struct ZenitScope *global_scope;
    struct ZenitScope *current_scope;
};

/*
 * Function: zenit_program_new
 *  Creates a new Zenit program object
 *
 * Returns:
 *  <struct ZenitProgram>*: The created program
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_program_free> function
 */
struct ZenitProgram* zenit_program_new(void);

/*
 * Function: zenit_program_free
 *  Releases the memory of the program object
 *
 * Parameters:
 *  <struct ZenitProgram> *program: Program object to be freed
 *
 * Returns:
 *  <void>: This function does not return a value
 * 
 */
void zenit_program_free(struct ZenitProgram *program);

/*
 * Function: zenit_program_add_symbol
 *  Adds a new symbol to the current scope
 *
 * Parameters:
 *  <struct ZenitProgram> *program: Program object
 *  <struct ZenitSymbol> *symbol: Symbol to be added
 * 
 * Returns:
 *  <struct ZenitSymbol>*: Added symbol
 * 
 */
struct ZenitSymbol* zenit_program_add_symbol(struct ZenitProgram *program, struct ZenitSymbol *symbol);

/*
 * Function: zenit_program_has_symbol
 *  Returns *true* if the program contains a symbol with name equals to the string *name*
 *
 * Parameters:
 *  <struct ZenitProgram> *program: Program object
 *  <const char> *name: Name of the symbol
 *
 * Returns:
 *  <bool>: *true* if there is a symbol with the provided *name* within the program object
 *
 */
bool zenit_program_has_symbol(struct ZenitProgram *program, const char *name);

/*
 * Function: zenit_program_get_symbol
 *  Returns the symbol with a name equals to the *symbol_name*, if such a symbol exists within the 
 *  program object
 *
 * Parameters:
 *  <struct ZenitProgram> *program: Program object
 *  <const char> *symbol_name: Name of the symbol to retrieve from the program
 *
 * Returns:
 *  <struct ZenitSymbol>*: A valid symbol if it exists, otherwise, this function returns NULL
 * 
 */
struct ZenitSymbol* zenit_program_get_symbol(struct ZenitProgram *program, const char *symbol_name);

/*
 * Function: zenit_program_remove_symbol
 *  This function removes the symbol that matches with the *symbol_name* from the program
 *  and returns the removed object
 *
 * Parameters:
 *  <struct ZenitProgram> *program: Program object
 *  <const char> *symbol_name: Name of the symbol to remove from the program
 *
 * Returns:
 *  struct ZenitSymbol*: Removed symbol
 *
 */
struct ZenitSymbol* zenit_program_remove_symbol(struct ZenitProgram *program, const char *symbol_name);

char* zenit_program_dump(struct ZenitProgram *program);

#endif /* ZENIT_PROGRAM_H */
