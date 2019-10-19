#ifndef CENIT_INFER_H
#define CENIT_INFER_H

#include "ast.h"
#include "context.h"

/*
 * Function: cenit_infer_types
 *  This function walks over the <CenitAst> tree and updates
 *  the <CenitSymbolTable> in the context with the type information 
 *  inferred for each symbol that does not declare or contain type 
 *  information
 *
 * Parameters:
 *  ctx - Context object
 *  ast - Parse program tree
 *
 * Returns:
 *  void - *true* on a pass without errors. Otheriwise, this function returns
 *          *false* and the *errors* property in the <CenitContext> object
 *          should contain the error(s) description
 * 
 * Notes:
 *  This function updates the type information but do not perform any type
 *  checking. At the end of this pass, we can say all the symbols contain
 *  the type information, therefore there shouldn't be symbols with the 
 *  <CENIT_TYPE_NONE> value present in the <CenitSymbolTable> entries.
 */
bool cenit_infer_types(CenitContext *ctx);

#endif /* CENIT_INFER_H */
