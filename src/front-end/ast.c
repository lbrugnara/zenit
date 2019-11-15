#include "ast.h"


/*
 * Function: free_node_array_entry
 *  This function takes a void pointer that is expected to be
 *  a pointer to a <struct ZenitNode> pointer
 */
static void free_node_array_entry(void *nodeptr)
{
    if (!nodeptr)
        return;

    struct ZenitNode *node = *(struct ZenitNode**)nodeptr;

    if (!node)
        return;

    zenit_node_free(node);
}

static void free_attribute_property(void *nodeptr)
{
    if (!nodeptr)
        return;

    struct ZenitAttributePropertyNode *node = (struct ZenitAttributePropertyNode*)nodeptr;

    if (!node)
        return;

    if (node->name)
        fl_cstring_free(node->name);

    zenit_node_free(node->value);
}

static inline void free_attribute_node(struct ZenitAttributeNode *attribute_node)
{
    if (!attribute_node)
        return;

    if (attribute_node->name)
        fl_cstring_free(attribute_node->name);

    if (attribute_node->properties)
        fl_array_free_each(attribute_node->properties, free_attribute_property);
}

/*
 * Function: free_identifier_node
 *  Releases the memory of the <struct ZenitIdentifierNode> object
 *
 * Parameters:
 *  identifier_node - Object to be freed.
 *
 * Returns:
 *  void - This function does not return a value
 */
static inline void free_identifier_node(struct ZenitIdentifierNode *identifier_node)
{
    if (!identifier_node)
        return;

    fl_cstring_free(identifier_node->name);
}

/*
 * Function: free_unary_ref_node
 *  Releases the memory of the <struct ZenitUnaryRefNode> object
 *
 * Parameters:
 *  ref_node - Object to be freed.
 *
 * Returns:
 *  void - This function does not return a value
 */
static inline void free_unary_ref_node(struct ZenitUnaryRefNode *ref_node)
{
    if (!ref_node)
        return;

    zenit_node_free(ref_node->expression);
}

/*
 f Function: free_array_initializer_node
 *  Releases the memory of a <struct ZenitArrayInitNode> object
 *
 * Parameters:
 *  array - <struct ZenitArrayInitNode> object
 *
 * Returns:
 *  void - This function does not return a value
 */
static inline void free_array_initializer_node(struct ZenitArrayInitNode *array)
{
    if (!array)
        return;

    zenit_node_array_free(array->elements);    
}

/*
 * Function: free_literal_node
 *  Frees the memory of a <struct ZenitLiteralNode> object
 *
 * Parameters:
 *  literal_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
static inline void free_literal_node(struct ZenitLiteralNode *literal_node)
{
    if (!literal_node)
        return;
}

/*
 * Function: free_variable_declaration_node
 *  Frees the memory of a <struct ZenitVariableNode> object
 *
 * Parameters:
 *  var_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
static inline void free_variable_declaration_node(struct ZenitVariableNode *var_node)
{
    if (!var_node)
        return;

    if (var_node->name)
        fl_cstring_free(var_node->name);

    if (var_node->value)
        zenit_node_free(var_node->value);

    if (var_node->attributes)
        fl_array_free_each(var_node->attributes, free_node_array_entry);
}

/*
 * Function: zenit_node_free
 *  Checks the node's *type* property to call the specific
 *  function that releases its memory.
 */
void zenit_node_free(struct ZenitNode *node)
{
    if (!node)
        return;

    if (node->type == ZENIT_NODE_LITERAL)
    {
        free_literal_node((struct ZenitLiteralNode*)node);
    }
    else if (node->type == ZENIT_NODE_VARIABLE)
    {
        free_variable_declaration_node((struct ZenitVariableNode*)node);
    }
    else if (node->type == ZENIT_NODE_ARRAY_INIT)
    {
        free_array_initializer_node((struct ZenitArrayInitNode*)node);
    }
    else if (node->type == ZENIT_NODE_UNARY_REF)
    {
        free_unary_ref_node((struct ZenitUnaryRefNode*)node);
    }
    else if (node->type == ZENIT_NODE_IDENTIFIER)
    {
        free_identifier_node((struct ZenitIdentifierNode*)node);
    }
    else if (node->type == ZENIT_NODE_ATTRIBUTE)
    {
        free_attribute_node((struct ZenitAttributeNode*)node);
    }

    zenit_type_free(&node->typeinfo);    

    fl_free(node);
}

/*
 * Function: zenit_node_array_free
 *  Frees the memory allocated by both the <struct ZenitNode> objects
 *  and the <FlArray>.
 */
void zenit_node_array_free(struct ZenitNode **array)
{
    if (!array)
        return;

    fl_array_free_each(array, free_node_array_entry);
}

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
