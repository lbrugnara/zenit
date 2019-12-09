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

char* zenit_node_type_array_uid(struct ZenitArrayTypeNode *type_node)
{
    if (!type_node)
        return NULL;

    char *format = "%%L%u_C%u_type_array";
    size_t length = snprintf(NULL, 0, format, type_node->base.base.location.line, type_node->base.base.location.col);
    char *id = fl_cstring_new(length);
    snprintf(id, length+1, format, type_node->base.base.location.line, type_node->base.base.location.col);
    id[length] = '\0';
    return id;
}

void zenit_node_type_array_free(struct ZenitArrayTypeNode *type_node)
{
    if (!type_node)
        return;

    zenit_node_free((struct ZenitNode*) type_node->members_type);

    fl_free(type_node);
}
