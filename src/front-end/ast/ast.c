#include "ast.h"

ZenitAst* zenit_ast_new(ZenitNode **decls)
{
    ZenitAst *ast = fl_malloc(sizeof(ZenitAst));
    ast->decls = decls;

    return ast;
}

char* zenit_ast_dump(ZenitAst *ast)
{
    char *output = fl_cstring_dup("(ast");

    if (ast->decls)
    {
        fl_cstring_append(&output, " ");
        size_t length = fl_array_length(ast->decls);
        for (size_t i=0; i < length; i++)
        {
            output = zenit_node_dump(ast->decls[i], output);

            if (i != length - 1)
                fl_cstring_append(&output, " ");
        }
    }

    fl_cstring_append(&output, ")");

    return output;
}

/*
 * Function: zenit_ast_free
 *  Frees the memory of all the declarations and the AST
 *  itself
 */
void zenit_ast_free(ZenitAst *ast)
{
    if (!ast)
        return;

    fl_array_free_each_pointer(ast->decls, (FlArrayFreeElementFunc) zenit_node_free);

    fl_free(ast);
}
