#include <fllib.h>
#include "type.h"
#include "reference.h"

struct ZenitReferenceTypeNode* zenit_node_type_reference_new(struct ZenitSourceLocation location, struct ZenitTypeNode *element_type)
{
    struct ZenitReferenceTypeNode *type_node = fl_malloc(sizeof(struct ZenitReferenceTypeNode));
    type_node->base.base.type = ZENIT_NODE_TYPE_REFERENCE;
    type_node->base.base.location = location;
    type_node->base.typekind = ZENIT_TYPE_REFERENCE;
    type_node->element = element_type;

    return type_node;
}

char* zenit_node_type_reference_uid(struct ZenitReferenceTypeNode *type_node)
{
    if (!type_node)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_type_reference", type_node->base.base.location.line, type_node->base.base.location.col);
}

char* zenit_node_type_reference_dump(struct ZenitReferenceTypeNode *type_node, char *output)
{
    char *type_str = zenit_node_type_reference_to_string(type_node);

    fl_cstring_vappend(&output, "(type %s)", type_str);

    fl_cstring_free(type_str);

    return output;
}

char* zenit_node_type_reference_to_string(struct ZenitReferenceTypeNode *type_node)
{
    if (type_node == NULL)
        return NULL;

    char *element = zenit_node_type_to_string(type_node->element);
    char *to_string = fl_cstring_vdup("&%s", element);
    fl_cstring_free(element);

    return to_string;
}

void zenit_node_type_reference_free(struct ZenitReferenceTypeNode *type_node)
{
    if (!type_node)
        return;

    zenit_node_free((struct ZenitNode*) type_node->element);

    fl_free(type_node);
}
