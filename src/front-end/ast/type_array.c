#include <fllib.h>
#include "type.h"
#include "type_array.h"

struct ZenitArrayTypeNode* zenit_node_type_array_new(struct ZenitSourceLocation location, struct ZenitTypeNode *members_type, size_t length)
{
    struct ZenitArrayTypeNode *type_node = fl_malloc(sizeof(struct ZenitArrayTypeNode));
    type_node->base.base.type = ZENIT_NODE_TYPE_ARRAY;
    type_node->base.base.location = location;
    type_node->base.type = ZENIT_TYPE_ARRAY;
    type_node->members_type = members_type;
    type_node->length = length;

    return type_node;
}

void zenit_node_type_array_free(struct ZenitArrayTypeNode *type_node)
{
    if (!type_node)
        return;

    zenit_node_free((struct ZenitNode*) type_node->members_type);

    fl_free(type_node);
}
