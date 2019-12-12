#include "ast.h"

/*
 * Function: zenit_ast_free
 *  Frees the memory of all the declarations and the AST
 *  itself
 */
void zenit_ast_free(struct ZenitAst *ast)
{
    if (!ast)
        return;

    fl_array_free_each_pointer(ast->decls, (FlArrayFreeElementFunc) zenit_node_free);

    fl_free(ast);
}
