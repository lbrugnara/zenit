#ifndef ZENIT_CHECK_H
#define ZENIT_CHECK_H

#include "../ast/ast.h"
#include "../context.h"

/*
 * Function: zenit_check_types
 *  This pass ensures the statements and expressions are valid based
 *  on the types involved in them.
 *
 * Parameters:
 *  ctx - Context object
 *
 * Returns:
 *  void - *true* on a pass without errors. Otheriwise, this function returns
 *          *false* and the *errors* property in the <struct ZenitContext> object
 *          should contain the error(s) description
 * 
 * Notes:
 *  This function only perform checks and does not update the symbol table. 
 *  At the end of this pass, we can say all the operations in the program
 *  are valid in regard to types
 */
bool zenit_check_types(struct ZenitContext *ctx);

#endif /* ZENIT_CHECK_H */
