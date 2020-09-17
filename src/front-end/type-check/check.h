#ifndef ZENIT_CHECK_H
#define ZENIT_CHECK_H

#include "../ast/ast.h"
#include "../context.h"

/*
 * Function: zenit_check_types
 *  This pass ensures that types of expressions are valid in the context they
 *  occur
 *
 * Parameters:
 *  <ZenitContext> *ctx - Context object
 *
 * Returns:
 *  void - *true* on a pass without errors. Otheriwise, this function returns
 *          *false* and the *errors* property in the <ZenitContext> object
 *          should contain the error(s) description
 * 
 * Notes:
 *  At the end of this pass, we can say all the operations in the program
 *  are valid in regard to types
 */
bool zenit_check_types(ZenitContext *ctx);

#endif /* ZENIT_CHECK_H */
