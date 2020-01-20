#ifndef ZENIT_INFER_H
#define ZENIT_INFER_H

#include "../ast/ast.h"
#include "../context.h"

/*
 * Function: zenit_infer_types
 *  This function does the type inference pass to update symbols that
 *  may not contain type information.
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
 *  This function updates the type information but does not perform any type
 *  checking. At the end of this pass, if there are not errors, we can say 
 *  all the symbols contain the type information, therefore there shouldn't 
 *  be symbols with the <ZENIT_TYPE_NONE> value present in the 
 *  <struct ZenitSymtable> entries.
 */
bool zenit_infer_types(struct ZenitContext *ctx);

#endif /* ZENIT_INFER_H */
