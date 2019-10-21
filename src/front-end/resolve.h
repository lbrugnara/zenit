#ifndef CENIT_RESOLVE_H
#define CENIT_RESOLVE_H

#include "ast.h"
#include "context.h"

/*
 * Function: cenit_resolve_symbols
 *  This function registers all the symbols defined in the source code
 *  in the symbol table
 *
 * Parameters:
 *  ctx - Context object
 *
 * Returns:
 *  void - *true* on a pass without errors. Otheriwise, this function returns
 *          *false* and the *errors* property in the <CenitContext> object
 *          should contain the error(s) description
 * 
 * Notes:
 *  This function just creates the <CenitSymbol> entries with the information
 *  present in the symbol definition, it does not perform any type of type 
 *  inference nor type checking
 */
bool cenit_resolve_symbols(CenitContext *ctx);

#endif /* CENIT_RESOLVE_H */
