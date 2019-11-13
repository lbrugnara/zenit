#ifndef ZENIT_PARSE_H
#define ZENIT_PARSE_H

#include "context.h"
#include "lexer.h"
#include "ast.h"

/*
 * Function: zenit_parse_source
 *  Parses the whole source code and populates the <struct ZenitContext>
 *  with the resulting <struct ZenitAst> object.
 *
 * Parameters:
 *  ctx - Context object
 *
 * Returns:
*  void - *true* tif he parsing ends without errors. Otheriwise, this function 
 *          returns *false* and the *errors* property in the <struct ZenitContext> 
 *          object should contain the error(s) description
 *
 */
bool zenit_parse_source(struct ZenitContext *ctx);

#endif /* ZENIT_PARSE_H */
