#ifndef ZENIT_SCOPE_H
#define ZENIT_SCOPE_H

#include "symtable.h"

/*
 * Struct: zenit_scope_new
 *  Represents a scope in the program that contains a symbol table
 * 
 * Members:
 *  <struct ZenitScope> *parent: Pointer to the parent scope
 *  <struct ZenitScope> **children: Set of children scopes
 *  <struct ZenitSymbolTable> symtable: Symbol table of the current scope
 * 
 */
struct ZenitScope {
    struct ZenitScope *parent;
    struct ZenitScope **children;
    struct ZenitSymbolTable symtable;
};

/*
 * Function: zenit_scope_new
 *  Creates a new scope object
 *
 * Parameters:
 *  <>  - 
 *
 * Returns:
 *  <struct ZenitScope>* - The created scope object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_scope_free> function
 */
struct ZenitScope* zenit_scope_new(const char *id, enum ZenitSymtableType type, struct ZenitScope *parent);

/*
 * Function: zenit_scope_free
 *  Releases the memory of the scope object
 *
 * Parameters:
 *  <struct ZenitScope> *scope - Scope to be freed
 *
 * Returns:
 *  void - This function does not return a value
 * 
 */
void zenit_scope_free(struct ZenitScope *scope);

#endif /* ZENIT_SCOPE_H */
