#ifndef CENIT_INFER_H
#define CENIT_INFER_H

#include "ast.h"
#include "context.h"

/*
 * Function: cenit_infer_types
 *  This function does the type inference pass to update symbols that
 *  may not contain type information.
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
 *  This function updates the type information but does not perform any type
 *  checking. At the end of this pass, if there are not errors, we can say 
 *  all the symbols contain the type information, therefore there shouldn't 
 *  be symbols with the <CENIT_TYPE_NONE> value present in the 
 *  <CenitSymbolTable> entries.
 */
bool cenit_infer_types(CenitContext *ctx);

#endif /* CENIT_INFER_H */
