#include <fllib.h>
#include "primitive.h"

struct ZenitPrimitiveNode* zenit_node_primitive_new(struct ZenitSourceLocation location, enum ZenitType type, union ZenitPrimitiveValue value)
{
    struct ZenitPrimitiveNode *primitive_node = fl_malloc(sizeof(struct ZenitPrimitiveNode));
    primitive_node->base.type = ZENIT_NODE_LITERAL;
    primitive_node->base.location = location;

    primitive_node->value = value;

    primitive_node->base.typeinfo.type = type;
    primitive_node->base.typeinfo.is_array = false;
    primitive_node->base.typeinfo.elements = 1;

    return primitive_node;
}

/*
 * Function: zenit_node_primitive_free
 *  Frees the memory of a <struct ZenitPrimitiveNode> object
 *
 * Parameters:
 *  primitive_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_primitive_free(struct ZenitPrimitiveNode *primitive_node)
{
    if (!primitive_node)
        return;

    fl_free(primitive_node);
}