#include <fllib.h>
#include "type.h"
#include "array.h"

struct ZenitArrayTypeNode* zenit_node_type_array_new(struct ZenitSourceLocation location, struct ZenitTypeNode *member_type)
{
    struct ZenitArrayTypeNode *type_node = fl_malloc(sizeof(struct ZenitArrayTypeNode));
    type_node->base.base.type = ZENIT_NODE_TYPE_ARRAY;
    type_node->base.base.location = location;
    type_node->base.type = ZENIT_TYPE_ARRAY;
    type_node->member_type = member_type;

    return type_node;
}

char* zenit_node_type_array_uid(struct ZenitArrayTypeNode *type_node)
{
    if (!type_node)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_type_array", type_node->base.base.location.line, type_node->base.base.location.col);
}

char* zenit_node_type_array_to_string(struct ZenitArrayTypeNode *type_node)
{
    if (type_node == NULL)
        return NULL;

    // We allocate memory for the string representation of this <struct ZenitTypeInfo> object
    char *string_value = fl_cstring_new(0);
    
    fl_cstring_append(&string_value, "[");
    if (!type_node->auto_length)
        fl_cstring_vappend(&string_value, "%zu", type_node->length);
    fl_cstring_append(&string_value, "]");

    char *member_type = zenit_node_type_to_string(type_node->member_type);
    fl_cstring_append(&string_value, member_type);
    fl_cstring_free(member_type);

    return string_value;
}

void zenit_node_type_array_free(struct ZenitArrayTypeNode *type_node)
{
    if (!type_node)
        return;

    zenit_node_free((struct ZenitNode*) type_node->member_type);

    fl_free(type_node);
}
