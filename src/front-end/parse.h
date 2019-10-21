#ifndef CENIT_PARSE_H
#define CENIT_PARSE_H

#include "context.h"
#include "lexer.h"
#include "ast.h"

/*
 * Function: cenit_parse_source
 *  Parses the whole source code and populates the <CenitContext>
 *  with the resulting <CenitAst> object.
 *
 * Parameters:
 *  ctx - Context object
 *
 * Returns:
*  void - *true* tif he parsing ends without errors. Otheriwise, this function 
 *          returns *false* and the *errors* property in the <CenitContext> 
 *          object should contain the error(s) description
 *
 */
bool cenit_parse_source(CenitContext *ctx);

#endif /* CENIT_PARSE_H */
