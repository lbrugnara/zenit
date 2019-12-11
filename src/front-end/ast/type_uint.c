#include <fllib.h>
#include "type.h"
#include "type_uint.h"

struct ZenitUintTypeNode* zenit_node_type_uint_new(struct ZenitSourceLocation location, enum ZenitUintTypeSize size)
{
    struct ZenitUintTypeNode *uint_type_node = fl_malloc(sizeof(struct ZenitUintTypeNode));
    uint_type_node->base.base.type = ZENIT_NODE_TYPE_PRIMITIVE;
    uint_type_node->base.base.location = location;
    uint_type_node->base.type = ZENIT_TYPE_UINT;
    uint_type_node->size = size;

    return uint_type_node;
}

char* zenit_node_type_uint_uid(struct ZenitUintTypeNode *uint_type_node)
{
    if (!uint_type_node)
        return NULL;

    size_t size = 0;
    if (uint_type_node->size == ZENIT_UINT_8)
        size = 8;
    else if (uint_type_node->size == ZENIT_UINT_16)
        size = 16;

    return fl_cstring_vdup("%%L%u_C%u_type_uint[%zu]", uint_type_node->base.base.location.line, uint_type_node->base.base.location.col, size);
}

void zenit_node_type_uint_free(struct ZenitUintTypeNode *uint_type_node)
{
    if (!uint_type_node)
        return;

    fl_free(uint_type_node);
}
