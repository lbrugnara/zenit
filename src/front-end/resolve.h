#ifndef CENIT_RESOLVE_H
#define CENIT_RESOLVE_H

#include "ast.h"
#include "symtable.h"

/*
 * Function: cenit_resolve_symbols
 *  This function walks over the <CenitAst> tree and populates
 *  the <CenitSymbolTable> with the symbols defined in the AST.
 *
 * Parameters:
 *  symtable - Symbol table
 *  ast - Parse program tree
 *
 * Returns:
 *  void - This function does not return a value
 * 
 * Notes:
 *  This function just creates the <CenitSymbol> entries with the information
 *  present in the symbol definition, it does not perform any type of type 
 *  inference nor type checking
 */
void cenit_resolve_symbols(CenitSymbolTable *symtable, CenitAst *ast);

#endif /* CENIT_RESOLVE_H */
