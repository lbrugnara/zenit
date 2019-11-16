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

    zenit_node_array_free(ast->decls);

    fl_free(ast);
}
