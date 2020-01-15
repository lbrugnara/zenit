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

bool zenit_scope_has_symbols(struct ZenitScope *scope);

char* zenit_scope_dump(struct ZenitScope *scope, char *output, bool verbose);

#endif /* ZENIT_SCOPE_H */
