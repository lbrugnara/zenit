#include "ast.h"


/*
 * Function: free_array_entry
 *  This function takes a void pointer that is expected to be
 *  a pointer to a <CenitNode> pointer
 */
void free_array_entry(void *nodeptr)
{
    if (!nodeptr)
        return;

    CenitNode *node = *(CenitNode**)nodeptr;

    if (!node)
        return;

    cenit_node_free(node);
}

/*
 * Function: array_initializer_node_free
 *  Releases the memory of a <CenitArrayInitNode> object
 *
 * Parameters:
 *  array - <CenitArrayInitNode> object
 *
 * Returns:
 *  void - This function does not return a value
 */
static inline void array_initializer_node_free(CenitArrayInitNode *array)
{
    cenit_node_array_free(array->values);

    if (array->typeinfo.name)
        fl_cstring_free(array->typeinfo.name);
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

/*
 * Function: cenit_node_free
 *  Checks the node's *type* property to call the specific
 *  function that releases its memory.
 */
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

/*
 * Function: cenit_node_array_free
 *  Frees the memory allocated by both the <CenitNode> objects
 *  and the <FlArray>.
 */
void cenit_node_array_free(CenitNode **array)
{
    if (!array)
        return;

    fl_array_free_each(array, free_array_entry);
}

/*
 * Function: cenit_ast_free
 *  Frees the memory of all the declarations and the AST
 *  itself
 */
void cenit_ast_free(CenitAst *ast)
{
    if (!ast)
        return;

    cenit_node_array_free(ast->decls);

    fl_free(ast);
}
