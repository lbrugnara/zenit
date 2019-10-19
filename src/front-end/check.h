#ifndef CENIT_CHECK_H
#define CENIT_CHECK_H

#include "ast.h"
#include "context.h"

/*
 * Function: cenit_check_types
 *  This function walks over the <CenitAst> tree and type checks
 *  all the symbols that are parte of assignments, initializers,
 *  conditionals, loops, etc. to make sure these operations are valid.
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
 *  This function only perform checks and does not update the symbol table. 
 *  At the end of this pass, we can say all the operations mentioned before
 *  are valid in regard to types
 */
bool cenit_check_types(CenitContext *ctx);

#endif /* CENIT_CHECK_H */
