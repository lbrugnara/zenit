
#include "type.h"
#include "uint.h"

ZenitUintTypeNode* zenit_uint_type_node_new(ZenitSourceLocation location, ZenitUintTypeSize size)
{
    ZenitUintTypeNode *uint_type_node = fl_malloc(sizeof(ZenitUintTypeNode));
    uint_type_node->base.base.nodekind = ZENIT_AST_NODE_TYPE_UINT;
    uint_type_node->base.base.location = location;
    uint_type_node->base.typekind = ZENIT_TYPE_UINT;
    uint_type_node->size = size;

    return uint_type_node;
}

char* zenit_uint_type_node_uid(ZenitUintTypeNode *uint_type_node)
{
    if (!uint_type_node)
        return NULL;

    size_t size = 0;
    if (uint_type_node->size == ZENIT_UINT_8)
        size = 8;
    else if (uint_type_node->size == ZENIT_UINT_16)
        size = 16;

    return fl_cstring_vdup("%%L%u:C%u_type_uint%zu", uint_type_node->base.base.location.line, uint_type_node->base.base.location.col, size);
}

char* zenit_uint_type_node_dump(ZenitUintTypeNode *type_node, char *output)
{
    char *type_str = zenit_uint_type_node_to_string(type_node);

    fl_cstring_vappend(&output, "(type %s)", type_str);

    fl_cstring_free(type_str);

    return output;
}

char* zenit_uint_type_node_to_string(ZenitUintTypeNode *uint_type_node)
{
    if (!uint_type_node)
        return NULL;

    if (uint_type_node->size == ZENIT_UINT_8)
        return fl_cstring_dup("uint8");

    if (uint_type_node->size == ZENIT_UINT_16)
        return fl_cstring_dup("uint16");

    return fl_cstring_dup("<unknown>");
}

void zenit_uint_type_node_free(ZenitUintTypeNode *uint_type_node)
{
    if (!uint_type_node)
        return;

    fl_free(uint_type_node);
}
