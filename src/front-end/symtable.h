#ifndef CENIT_SYMTABLE_H
#define CENIT_SYMTABLE_H

#include <fllib.h>
#include "symbol.h"

typedef enum CenitSymbolTableType {
    CENIT_SYMTABLE_GLOBAL,
    CENIT_SYMTABLE_FUNCTION,
} CenitSymbolTableType;

typedef struct CenitSymbolTable {
    const char *id;
    FlHashtable symbols;
    CenitSymbolTableType type;
} CenitSymbolTable;

CenitSymbolTable cenit_symtable_new(CenitSymbolTableType, const char *id);
void cenit_symtable_free(CenitSymbolTable *symtable);
CenitSymbol* cenit_symtable_add(CenitSymbolTable *symtable, CenitSymbol *symbol);
bool cenit_symtable_has(CenitSymbolTable *symtable, const char *symbol_name);
CenitSymbol* cenit_symtable_get(CenitSymbolTable *symtable, const char *symbol_name);

#endif /* CENIT_SYMTABLE_H */
