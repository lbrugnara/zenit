
#include <fllib/Array.h>
#include "type.h"
#include "struct.h"

ZenitStructTypeNode* zenit_struct_type_node_new(ZenitSourceLocation location, char *name)
{
    ZenitStructTypeNode *type_node = fl_malloc(sizeof(ZenitStructTypeNode));
    type_node->base.base.nodekind = ZENIT_NODE_TYPE_STRUCT;
    type_node->base.base.location = location;
    type_node->base.typekind = ZENIT_TYPE_STRUCT;
    type_node->name = name;
    type_node->members = fl_array_new(sizeof(ZenitTypeNode*), 0);

    return type_node;
}

char* zenit_struct_type_node_uid(ZenitStructTypeNode *type_node)
{
    if (!type_node)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_type_struct", type_node->base.base.location.line, type_node->base.base.location.col);
}

char* zenit_struct_type_node_dump(ZenitStructTypeNode *type_node, char *output)
{
    char *type_str = zenit_struct_type_node_to_string(type_node);

    fl_cstring_vappend(&output, "(type %s)", type_str);

    fl_cstring_free(type_str);

    return output;
}

char* zenit_struct_type_node_to_string(ZenitStructTypeNode *type_node)
{
    if (type_node == NULL)
        return NULL;

    return fl_cstring_dup(type_node->name);
}

void zenit_struct_type_node_free(ZenitStructTypeNode *type_node)
{
    if (!type_node)
        return;

    if (type_node->name)
        fl_cstring_free(type_node->name);

    if (type_node->members)
        fl_array_free_each_pointer(type_node->members, (FlArrayFreeElementFunc) zenit_node_free);

    fl_free(type_node);
}
