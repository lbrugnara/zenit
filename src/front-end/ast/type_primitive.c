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

char* zenit_node_type_primitive_uid(struct ZenitPrimitiveTypeNode *type_node)
{
    if (!type_node)
        return NULL;

    char *format = "%%L%u_C%u_type_primitive";
    size_t length = snprintf(NULL, 0, format, type_node->base.base.location.line, type_node->base.base.location.col);
    char *id = fl_cstring_new(length);
    snprintf(id, length+1, format, type_node->base.base.location.line, type_node->base.base.location.col);
    id[length] = '\0';
    return id;
}

void zenit_node_type_primitive_free(struct ZenitPrimitiveTypeNode *type_node)
{
    if (!type_node)
        return;

    fl_free(type_node);
}
