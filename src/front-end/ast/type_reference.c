#include <fllib.h>
#include "type.h"
#include "type_reference.h"

struct ZenitReferenceTypeNode* zenit_node_type_reference_new(struct ZenitSourceLocation location, struct ZenitTypeNode *element_type)
{
    struct ZenitReferenceTypeNode *type_node = fl_malloc(sizeof(struct ZenitReferenceTypeNode));
    type_node->base.base.type = ZENIT_NODE_TYPE_REFERENCE;
    type_node->base.base.location = location;
    type_node->base.type = ZENIT_TYPE_REFERENCE;
    type_node->element = element_type;

    return type_node;
}

char* zenit_node_type_reference_uid(struct ZenitReferenceTypeNode *type_node)
{
    if (!type_node)
        return NULL;

    char *format = "%%L%u_C%u_type_reference";
    size_t length = snprintf(NULL, 0, format, type_node->base.base.location.line, type_node->base.base.location.col);
    char *id = fl_cstring_new(length);
    snprintf(id, length+1, format, type_node->base.base.location.line, type_node->base.base.location.col);
    id[length] = '\0';
    return id;
}

void zenit_node_type_reference_free(struct ZenitReferenceTypeNode *type_node)
{
    if (!type_node)
        return;

    zenit_node_free((struct ZenitNode*) type_node->element);

    fl_free(type_node);
}
