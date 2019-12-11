#include <fllib.h>
#include "uint.h"
#include "../types/uint.h"

struct ZenitUintNode* zenit_node_uint_new(struct ZenitSourceLocation location, enum ZenitUintTypeSize size, union ZenitUintValue value)
{
    struct ZenitUintNode *uint_node = fl_malloc(sizeof(struct ZenitUintNode));
    uint_node->base.type = ZENIT_NODE_UINT;
    uint_node->base.location = location;
    uint_node->size = size;
    uint_node->value = value;

    return uint_node;
}


char* zenit_node_uint_uid(struct ZenitUintNode *uint)
{
    if (!uint)
        return NULL;

    size_t value = 0;

    switch (uint->size)
    {
        case ZENIT_UINT_8:
            value = (size_t) uint->value.uint8;
            break;

        case ZENIT_UINT_16:
            value = (size_t) uint->value.uint16;
            break;

        default:
            return NULL;
    }

    return fl_cstring_vdup("%%L%u_C%u_uint[n:%zu]", uint->base.location.line, uint->base.location.col, value);
}

/*
 * Function: zenit_node_uint_free
 *  Frees the memory of a <struct ZenitUintNode> object
 *
 * Parameters:
 *  uint_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_uint_free(struct ZenitUintNode *uint_node)
{
    if (!uint_node)
        return;

    fl_free(uint_node);
}
