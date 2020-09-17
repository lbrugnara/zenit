
#include <fllib/Array.h>
#include <fllib/Cstring.h>
#include "struct.h"

ZenitStructNode* zenit_struct_node_new(ZenitSourceLocation location, char *name)
{
    ZenitStructNode *struct_node = fl_malloc(sizeof(ZenitStructNode));
    struct_node->base.nodekind = ZENIT_AST_NODE_STRUCT;
    struct_node->base.location = location;
    struct_node->name = name;
    struct_node->members = fl_array_new(sizeof(ZenitNode*), 0);

    return struct_node;
}

char* zenit_struct_node_uid(ZenitStructNode *struct_node)
{
    if (!struct_node)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_struct", struct_node->base.location.line, struct_node->base.location.col);
}

char* zenit_struct_node_dump(ZenitStructNode *struct_node, char *output)
{
    if (struct_node->name != NULL)
        fl_cstring_vappend(&output, "(struct %s ", struct_node->name);
    else
        fl_cstring_append(&output, "(struct ");

    size_t length = fl_array_length(struct_node->members);
    for (size_t i=0; i < length; i++)
    {
        output = zenit_node_dump(struct_node->members[i], output);
        if (i != length - 1)
            fl_cstring_append(&output, " ");
    }

    fl_cstring_append(&output, ")");

    return output;
}

/*
 * Function: zenit_struct_node_free
 *  Frees the memory of a <ZenitStructNode> object
 *
 * Parameters:
 *  struct_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_struct_node_free(ZenitStructNode *struct_node)
{
    if (!struct_node)
        return;

    if (struct_node->name)
        fl_cstring_free(struct_node->name);

    fl_array_free_each_pointer(struct_node->members, (FlArrayFreeElementFunc) zenit_node_free);

    fl_free(struct_node);
}
