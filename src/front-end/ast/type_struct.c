#include <fllib.h>
#include "type.h"
#include "type_struct.h"

struct ZenitStructTypeNode* zenit_node_type_struct_new(struct ZenitSourceLocation location, char *name)
{
    struct ZenitStructTypeNode *type_node = fl_malloc(sizeof(struct ZenitStructTypeNode));
    type_node->base.base.type = ZENIT_NODE_TYPE_STRUCT;
    type_node->base.base.location = location;
    type_node->base.type = ZENIT_TYPE_STRUCT;
    type_node->name = fl_cstring_dup(name);
    type_node->members = fl_array_new(sizeof(struct ZenitTypeNode*), 0);

    return type_node;
}

void zenit_node_type_struct_free(struct ZenitStructTypeNode *type_node)
{
    if (!type_node)
        return;

    if (type_node->name)
        fl_cstring_free(type_node->name);

    if (type_node->members)
        fl_array_free_each_pointer(type_node->members, (FlArrayFreeElementFunc)zenit_node_free);

    fl_free(type_node);
}
