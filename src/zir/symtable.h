#ifndef ZENIT_IR_SYMTABLE_H
#define ZENIT_IR_SYMTABLE_H

#include <fllib.h>
#include "symbol.h"

/*
 * Enum: enum ZenitIrSymbolTableType
 *  Represents the different type of symbol tables
 * 
 */
enum ZenitIrSymbolTableType {
    ZENIT_IR_SYMTABLE_GLOBAL,
    ZENIT_IR_SYMTABLE_FUNCTION,
};

/*
 * Struct: struct ZenitIrSymbolTable
 *  A symbol table object that keeps track of the program's symbols
 * 
 * Members:
 *  <const char> *id: Symbol table identifier
 *  <FlHashtable> symbols: Hashtable of symbols using the name (string) as key
 *  <enum ZenitIrSymbolTableType> type: The type of symbol table
 * 
 */
struct ZenitIrSymbolTable {
    const char *id;
    FlHashtable symbols;
    enum ZenitIrSymbolTableType type;
};

/*
 * Function: zenit_ir_symtable_new
 *  Creates a new symbol table
 *
 * Parameters:
 *  <enum ZenitIrSymbolTableType> type - The type of symbol table
 *  <const char> *id: An identifier for the symbol table
 *
 * Returns:
 *  <struct ZenitIrSymbolTable> - The created symbol table
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_ir_symtable_free> function
 */
struct ZenitIrSymbolTable zenit_ir_symtable_new(enum ZenitIrSymbolTableType, const char *id);

/*
 * Function: zenit_ir_symtable_free
 *  Releases the memory allocated in the *symtable* object
 *
 * Parameters:
 *  <struct ZenitIrSymbolTable> *symtable - Symbol table to be freed
 *
 * Returns:
 *  void - This function does not return a value
 * 
 */
void zenit_ir_symtable_free(struct ZenitIrSymbolTable *symtable);

/*
 * Function: zenit_ir_symtable_add
 *  Adds the <struct ZenitIrSymbol> object to the symbol table
 *
 * Parameters:
 *  <struct ZenitIrSymbolTable> *symtable - The symbol table
 *  <struct ZenitIrSymbol> *symbol - The symbol to be added to the symbol table
 *
 * Returns:
 *  <struct ZenitIrSymbol>* - The symbol object
 *
 */
struct ZenitIrSymbol* zenit_ir_symtable_add(struct ZenitIrSymbolTable *symtable, struct ZenitIrSymbol *symbol);

/*
 * Function: zenit_ir_symtable_has
 *  Checks if a symbol exists in the symbol table
 *
 * Parameters:
 *  <struct ZenitIrSymbolTable> *symtable - The symbol table
 *  <const char> *symbol_name - The symbol's name to look for
 *
 * Returns:
 *  bool - *true* if there is a symbol with the provided name, otherwise this function
 *          returns *false*.
 *
 */
bool zenit_ir_symtable_has(struct ZenitIrSymbolTable *symtable, const char *symbol_name);

/*
 * Function: zenit_ir_symtable_get
 *  This function returns a symbol from the symbol table if there is a value for the provided *symbol_name* key
 *
 * Parameters:
 *  <struct ZenitIrSymbolTable> *symtable - Symbol table
 *  <const char> *symbol_name - Key to lookup the symbol
 *
 * Returns:
 *  <struct ZenitIrSymbol>* - Pointer to the symbol object or NULL
 *
 */
struct ZenitIrSymbol* zenit_ir_symtable_get(struct ZenitIrSymbolTable *symtable, const char *symbol_name);

struct ZenitIrSymbol** zenit_ir_symtable_get_all(struct ZenitIrSymbolTable *symtable);

#endif /* ZENIT_IR_SYMTABLE_H */
