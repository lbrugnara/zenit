#include <fllib.h>
#include "struct.h"

struct ZenitStructNode* zenit_node_struct_new(struct ZenitSourceLocation location, char *name)
{
    struct ZenitStructNode *struct_node = fl_malloc(sizeof(struct ZenitStructNode));
    struct_node->base.type = ZENIT_NODE_STRUCT;
    struct_node->base.location = location;
    struct_node->name = name;
    struct_node->members = fl_array_new(sizeof(struct ZenitNode*), 0);

    return struct_node;
}

char* zenit_node_struct_uid(struct ZenitStructNode *struct_node)
{
    if (!struct_node)
        return NULL;

    char *id = fl_cstring_vdup("%%L%u:C%u_struct[n:%s]", struct_node->base.location.line, struct_node->base.location.col, struct_node->name);

    return id;
}

char* zenit_node_struct_dump(struct ZenitStructNode *struct_node, char *output)
{
    fl_cstring_vappend(&output, "(struct %s ", struct_node->name);

    size_t length = fl_array_length(struct_node->members);
    for (size_t i=0; i < length; i++)
    {
        output = zenit_node_dump(struct_node->members[i], output);
        if (i != length - 1)
            fl_cstring_append(&output, " ");
    }

    struct ZenitAttributeNode **attrs = zenit_attribute_node_map_values(&struct_node->attributes);

    length = fl_array_length(attrs);
    if (length > 0)
    {
        fl_cstring_append(&output, " ");

        for (size_t i=0; i < length; i++)
        {
            output = zenit_node_attribute_dump(attrs[i], output);
            if (i != length - 1)
                fl_cstring_append(&output, " ");
        }
    }

    fl_array_free(attrs);

    fl_cstring_append(&output, ")");

    return output;
}

/*
 * Function: zenit_node_struct_free
 *  Frees the memory of a <struct ZenitStructNode> object
 *
 * Parameters:
 *  struct_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_struct_free(struct ZenitStructNode *struct_node)
{
    if (!struct_node)
        return;

    if (struct_node->name)
        fl_cstring_free(struct_node->name);

    fl_array_free_each_pointer(struct_node->members, (FlArrayFreeElementFunc) zenit_node_free);

    zenit_attribute_node_map_free(&struct_node->attributes);

    fl_free(struct_node);
}
