#include <fllib.h>
#include "type.h"
#include "type_primitive.h"

struct ZenitPrimitiveTypeNode* zenit_node_type_primitive_new(struct ZenitSourceLocation location, enum ZenitType type)
{
    if (!zenit_type_is_primitive(type))
        return NULL;

    struct ZenitPrimitiveTypeNode *type_node = fl_malloc(sizeof(struct ZenitPrimitiveTypeNode));
    type_node->base.base.type = ZENIT_NODE_TYPE_PRIMITIVE;
    type_node->base.base.location = location;
    type_node->base.type = type;

    return type_node;
}

void zenit_node_type_primitive_free(struct ZenitPrimitiveTypeNode *type_node)
{
    if (!type_node)
        return;

    fl_free(type_node);
}
