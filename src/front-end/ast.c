#include "ast.h"

static inline void array_initializer_node_free(CenitArrayInitNode *array)
{
    if (array->values)
        fl_array_free_each(array->values, cenit_node_pointer_free);
}

/*
 * Function: literal_node_free
 *  Frees the memory of a <CenitLiteralNode> object
 *
 * Parameters:
 *  literal_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
static inline void literal_node_free(CenitLiteralNode *literal_node)
{
    if (literal_node->typeinfo.name)
        fl_cstring_free(literal_node->typeinfo.name);
}

/*
 * Function: variable_declaration_node_free
 *  Frees the memory of a <CenitVariableNode> object
 *
 * Parameters:
 *  var_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
static inline void variable_declaration_node_free(CenitVariableNode *var_node)
{
    if (var_node->name)
        fl_cstring_free(var_node->name);

    if (var_node->typeinfo.name)
        fl_cstring_free(var_node->typeinfo.name);

    if (var_node->value)
        cenit_node_free(var_node->value);
}

void cenit_node_free(CenitNode *node)
{
    if (!node)
        return;

    if (node->type == CENIT_NODE_LITERAL)
    {
        literal_node_free((CenitLiteralNode*)node);
    }
    else if (node->type == CENIT_NODE_VARIABLE)
    {
        variable_declaration_node_free((CenitVariableNode*)node);
    }
    else if (node->type == CENIT_NODE_ARRAY_INIT)
    {
        array_initializer_node_free((CenitArrayInitNode*)node);
    }

    fl_free(node);
}

void cenit_node_pointer_free(void *nodeptr)
{
    if (!nodeptr)
        return;

    CenitNode *node = *(CenitNode**)nodeptr;

    if (!node)
        return;

    cenit_node_free(node);
}

void cenit_ast_free(CenitAst *ast)
{
    if (!ast || !ast->decls)
        return;

    fl_array_free_each(ast->decls, cenit_node_pointer_free);

    fl_free(ast);
}
