#ifndef CENIT_PARSE_H
#define CENIT_PARSE_H

#include "context.h"
#include "lexer.h"
#include "ast.h"

/*
 * Function: cenit_parse_string
 *  Parses the whole source code and populates the <CenitContext>
 *  with a <CenitAst> object.
 *
 * Parameters:
 *  ctx - Context object
 *  source - Source code
 *
 * Returns:
*  void - *true* tif he parsing ends without errors. Otheriwise, this function 
 *          returns *false* and the *errors* property in the <CenitContext> 
 *          object should contain the error(s) description
 *
 */
bool cenit_parse_string(CenitContext *ctx, const char *source);

#endif /* CENIT_PARSE_H */
