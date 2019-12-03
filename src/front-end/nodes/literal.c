#include <fllib.h>
#include "literal.h"

struct ZenitLiteralNode* zenit_node_literal_new(struct ZenitSourceLocation location, enum ZenitType type, union ZenitLiteralValue value)
{
    struct ZenitLiteralNode *literal_node = fl_malloc(sizeof(struct ZenitLiteralNode));
    literal_node->base.type = ZENIT_NODE_LITERAL;
    literal_node->base.location = location;

    literal_node->value = value;

    literal_node->base.typeinfo.type = type;
    literal_node->base.typeinfo.is_array = false;
    literal_node->base.typeinfo.elements = 1;

    return literal_node;
}

/*
 * Function: zenit_node_literal_free
 *  Frees the memory of a <struct ZenitLiteralNode> object
 *
 * Parameters:
 *  literal_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_literal_free(struct ZenitLiteralNode *literal_node)
{
    if (!literal_node)
        return;

    fl_free(literal_node);
}
