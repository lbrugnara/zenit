
#include "type.h"
#include "bool.h"

ZenitBoolTypeNode* zenit_node_type_bool_new(ZenitSourceLocation location)
{
    ZenitBoolTypeNode *bool_type_node = fl_malloc(sizeof(ZenitBoolTypeNode));
    bool_type_node->base.base.nodekind = ZENIT_NODE_TYPE_BOOL;
    bool_type_node->base.base.location = location;
    bool_type_node->base.typekind = ZENIT_TYPE_BOOL;

    return bool_type_node;
}

char* zenit_node_type_bool_uid(ZenitBoolTypeNode *bool_type_node)
{
    if (!bool_type_node)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_type_bool", bool_type_node->base.base.location.line, bool_type_node->base.base.location.col);
}

char* zenit_node_type_bool_dump(ZenitBoolTypeNode *type_node, char *output)
{
    char *type_str = zenit_node_type_bool_to_string(type_node);

    fl_cstring_vappend(&output, "(type %s)", type_str);

    fl_cstring_free(type_str);

    return output;
}

char* zenit_node_type_bool_to_string(ZenitBoolTypeNode *bool_type_node)
{
    if (!bool_type_node)
        return NULL;

    return fl_cstring_dup("bool");
}

void zenit_node_type_bool_free(ZenitBoolTypeNode *bool_type_node)
{
    if (!bool_type_node)
        return;

    fl_free(bool_type_node);
}
