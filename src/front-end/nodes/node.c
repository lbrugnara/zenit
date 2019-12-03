#include "array.h"
#include "attribute.h"
#include "identifier.h"
#include "literal.h"
#include "property.h"
#include "reference.h"
#include "variable.h"

/*
 * Function: free_node_array_entry
 *  This function takes a void pointer that is expected to be
 *  a pointer to a <struct ZenitNode> pointer
 */
static inline void free_node_array_entry(void *nodeptr)
{
    if (!nodeptr)
        return;

    struct ZenitNode *node = *(struct ZenitNode**)nodeptr;

    if (!node)
        return;

    zenit_node_free(node);
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

    zenit_type_free(&node->typeinfo);

    if (node->type == ZENIT_NODE_LITERAL)
    {
        zenit_node_literal_free((struct ZenitLiteralNode*)node);
    }
    else if (node->type == ZENIT_NODE_VARIABLE)
    {
        zenit_node_variable_free((struct ZenitVariableNode*)node);
    }
    else if (node->type == ZENIT_NODE_ARRAY_INIT)
    {
        zenit_node_array_free((struct ZenitArrayNode*)node);
    }
    else if (node->type == ZENIT_NODE_UNARY_REF)
    {
        zenit_node_unary_ref_free((struct ZenitUnaryRefNode*)node);
    }
    else if (node->type == ZENIT_NODE_IDENTIFIER)
    {
        zenit_node_identifier_free((struct ZenitIdentifierNode*)node);
    }
    else if (node->type == ZENIT_NODE_ATTRIBUTE)
    {
        zenit_node_attribute_free((struct ZenitAttributeNode*)node);
    }
    else if (node->type == ZENIT_NODE_ATTRIBUTE_PROPERTY)
    {
        zenit_node_property_free((struct ZenitPropertyNode*)node);
    }
}
