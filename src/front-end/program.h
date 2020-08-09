#ifndef ZENIT_PROGRAM_H
#define ZENIT_PROGRAM_H

#include "scope.h"

/*
 * Struct: ZenitProgram
 *  Represents a Zenit program
 * 
 * Members:
 *  <ZenitScope> *global: A pointer to the global scope
 *  <ZenitScope> *current: A pointer to the current scope
 */
typedef struct ZenitProgram {
    ZenitScope *global_scope;
    ZenitScope *current_scope;
} ZenitProgram;

/*
 * Function: zenit_program_new
 *  Creates a new Zenit program object
 *
 * Returns:
 *  <ZenitProgram>*: The created program
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_program_free> function
 */
ZenitProgram* zenit_program_new(void);

/*
 * Function: zenit_program_free
 *  Releases the memory of the program object
 *
 * Parameters:
 *  <ZenitProgram> *program: Program object to be freed
 *
 * Returns:
 *  <void>: This function does not return a value
 * 
 */
void zenit_program_free(ZenitProgram *program);

/*
 * Function: zenit_program_add_scope
 *  Adds a new scope -of the provided type, and with the provided name- as a child of the current scope
 *
 * Parameters:
 *  <ZenitProgram> *program: Program object
 *  <ZenitScopeType> type: Type of the scope to be created
 *  <const char> *name: Name of the scope to be created
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_program_add_scope(ZenitProgram *program, ZenitScopeType type, const char *name);

/*
 * Function: zenit_program_enter_scope
 *  Sets the provided scope as the current scope in the program as long as the current scope is the
 *  parent of the provided scope
 *
 * Parameters:
 *  <ZenitProgram> *program: Program object
 *  <ZenitScope> *scope: Target scope 
 *
 * Returns:
 *  bool: *true* if the provided scope can be selected as the current scope, otherwise *false*.
 */
bool zenit_program_enter_scope(ZenitProgram *program, ZenitScope *scope);

/*
 * Function: zenit_program_push_scope
 *  This function sets an scope of the provided type and with the provided name as the current scope within the 
 *  program. If the scope does not exist, this function creates it. In case the scope already exists, this function
 *  uses that scope as the target scope to be selected as "current".
 *
 * Parameters:
 *  <ZenitProgram> *program: Program object
 *  <ZenitScopeType> typ: Type of the target scope
 *  <const char> *name: Name of the target scope
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_program_push_scope(ZenitProgram *program, ZenitScopeType type, const char *name);

/*
 * Function: zenit_program_pop_scope
 *  Changes the current scope by selecting the current scope's parent as the new current scope
 *
 * Parameters:
 *  <ZenitProgram> *program: Program object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_program_pop_scope(ZenitProgram *program);

/*
 * Function: zenit_program_has_scope
 *  Returns *true* or *false* based on if a scope with the provided type and name exists in the program and
 *  can be reached from the current scope.
 *
 * Parameters:
 *  <ZenitProgram> *program: Program object
 *  <ZenitScopeType> type: Scope type
 *  <const char> *name: Scope name
 *
 * Returns:
 *  bool: *true* if a scope with that type and name exists, otherwise *false*.
 */
bool zenit_program_has_scope(ZenitProgram *program, ZenitScopeType type, const char *name);

/*
 * Function: zenit_program_get_scope
 *  Returns -if it exists- an scope of the provided type and provided name that is reachable from the current scope
 *
 * Parameters:
 *  <ZenitProgram> *program: Program object
 *  <ZenitScopeType> type: Type of the target scope
 *  <const char> *name: Name of the target scope
 *
 * Returns:
 *  ZenitScope*: Pointer to the target scope if it exists, otherwise this function returns <NULL>
 */
ZenitScope* zenit_program_get_scope(ZenitProgram *program, ZenitScopeType type, const char *name);

/*
 * Function: zenit_program_add_symbol
 *  Adds a new symbol to the current scope
 *
 * Parameters:
 *  <ZenitProgram> *program: Program object
 *  <ZenitSymbol> *symbol: Symbol to be added
 * 
 * Returns:
 *  <ZenitSymbol>*: Added symbol
 * 
 * Notes:
 *  The program object takes ownership of the symbol object, which means that the memory of the symbol object is
 *  freed when the program object is freed
 */
ZenitSymbol* zenit_program_add_symbol(ZenitProgram *program, ZenitSymbol *symbol);

/*
 * Function: zenit_program_has_symbol
 *  Returns *true* if the program contains a symbol with name equals to the string *name*
 *
 * Parameters:
 *  <ZenitProgram> *program: Program object
 *  <const char> *name: Name of the symbol
 *
 * Returns:
 *  <bool>: *true* if there is a symbol with the provided *name* within the program object
 *
 */
bool zenit_program_has_symbol(ZenitProgram *program, const char *name);

/*
 * Function: zenit_program_get_symbol
 *  Returns the symbol with a name equals to the *symbol_name*, if such a symbol exists within the 
 *  program object
 *
 * Parameters:
 *  <ZenitProgram> *program: Program object
 *  <const char> *symbol_name: Name of the symbol to retrieve from the program
 *
 * Returns:
 *  <ZenitSymbol>*: A valid symbol if it exists, otherwise, this function returns NULL
 * 
 */
ZenitSymbol* zenit_program_get_symbol(ZenitProgram *program, const char *symbol_name);

/*
 * Function: zenit_program_remove_symbol
 *  This function removes the symbol that matches with the *symbol_name* from the program
 *  and returns the removed object
 *
 * Parameters:
 *  <ZenitProgram> *program: Program object
 *  <const char> *symbol_name: Name of the symbol to remove from the program
 *
 * Returns:
 *  ZenitSymbol*: Removed symbol
 *
 * Notes:
 *  The program object loses ownership of the symbol being removed, which means the caller is in charge
 *  of freeing the symbol's memory
 */
ZenitSymbol* zenit_program_remove_symbol(ZenitProgram *program, const char *symbol_name);

/*
 * Function: zenit_program_dump
 *  Returns a heap allocated string containing a dump of the program object
 *
 * Parameters:
 *  <ZenitProgram> *program: Program object to dump to the output
 *  <bool> verbose: If true, the temporal symbols are added to the output, otherwise they are ignored
 *
 * Returns:
 *  char*: Pointer to a heap allocated string containing the dump of the program
 *
 * Notes:
 *  The string returned by this function must be freed with the <fl_cstring_free> function
 */
char* zenit_program_dump(ZenitProgram *program, bool verbose);

/*
 * Function: zenit_program_is_valid_type
 *  Inspects the type object to know if it is valid in the current context of the program. For primitive types
 *  this function always returns true. For compound types (arrays, structs, etc) this function recursively checks
 *  its components to know if they are valid (are defined).
 *
 * Parameters:
 *  <ZenitProgram> *program: Program object
 *  <ZenitType> *type: Type object
 *
 * Returns:
 *  bool: *true* if the type is valid, otherwise this function returns *false*
 *
 * Notes:
 *  If this function returns *false*, the function <zenit_program_get_invalid_type_component> can be used to
 *  retrieve the specific type that is not valid in the current context.
 */
bool zenit_program_is_valid_type(ZenitProgram *program, ZenitType *type);

/*
 * Function: zenit_program_get_invalid_type_component
 *  Inspects the type object (recursively for compound types) up to find an invalid component.
 *
 * Parameters:
 *  <ZenitProgram> *program: Program object
 *  <ZenitType> *type: Type object
 *
 * Returns:
 *  ZenitType*: Invalid component of the type object or NULL if the type is valid
 *
 * Notes:
 *  The function <zenit_program_is_valid_type> can be used to know if the type object contains invalid components
 */
ZenitType* zenit_program_get_invalid_type_component(ZenitProgram *program, ZenitType *type);

#endif /* ZENIT_PROGRAM_H */
