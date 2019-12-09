#include <fllib.h>
#include "primitive.h"
#include "../types/primitive.h"

struct ZenitPrimitiveNode* zenit_node_primitive_new(struct ZenitSourceLocation location, enum ZenitType type, union ZenitPrimitiveValue value)
{
    struct ZenitPrimitiveNode *primitive_node = fl_malloc(sizeof(struct ZenitPrimitiveNode));
    primitive_node->base.type = ZENIT_NODE_PRIMITIVE;
    primitive_node->base.location = location;
    primitive_node->type = type;
    primitive_node->value = value;

    return primitive_node;
}


char* zenit_node_primitive_uid(struct ZenitPrimitiveNode *primitive)
{
    if (!primitive)
        return NULL;

    size_t uint = 0;

    switch (primitive->type)
    {
        case ZENIT_TYPE_UINT8:
            uint = (size_t) primitive->value.uint8;
            break;

        case ZENIT_TYPE_UINT16:
            uint = (size_t) primitive->value.uint16;
            break;

        default:
            return NULL;
    }

    char *format = "%%L%u_C%u_primitive[n:%zu]";
    size_t length = snprintf(NULL, 0, format, primitive->base.location.line, primitive->base.location.col, uint);
    char *id = fl_cstring_new(length);
    snprintf(id, length+1, format, primitive->base.location.line, primitive->base.location.col, uint);
    id[length] = '\0';
    return id;
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
