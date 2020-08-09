#ifndef ZENIT_RESOLVE_H
#define ZENIT_RESOLVE_H

#include "../ast/ast.h"
#include "../context.h"

/*
 * Function: zenit_resolve_symbols
 *  This function registers all the symbols defined in the source code
 *  in the symbol table
 *
 * Parameters:
 *  ctx - Context object
 *
 * Returns:
 *  void - *true* on a pass without errors. Otheriwise, this function returns
 *          *false* and the *errors* property in the <ZenitContext> object
 *          should contain the error(s) description
 * 
 * Notes:
 *  This function creates the <ZenitSymbol> entries with the information
 *  present in the symbol definition, and performs some basic checks on user-defined
 *  types
 */
bool zenit_resolve_symbols(ZenitContext *ctx);

#endif /* ZENIT_RESOLVE_H */
