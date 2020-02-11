
#include <fllib/Cstring.h>
#include "bool.h"
#include "../types/bool.h"

struct ZenitBoolNode* zenit_node_bool_new(struct ZenitSourceLocation location, bool value)
{
    struct ZenitBoolNode *bool_node = fl_malloc(sizeof(struct ZenitBoolNode));
    bool_node->base.nodekind = ZENIT_NODE_BOOL;
    bool_node->base.location = location;
    bool_node->value = value;

    return bool_node;
}

char* zenit_node_bool_uid(struct ZenitBoolNode *bool_node)
{
    if (!bool_node)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_bool", bool_node->base.location.line, bool_node->base.location.col);
}

char* zenit_node_bool_dump(struct ZenitBoolNode *bool_node, char *output)
{
    fl_cstring_vappend(&output, "(bool %s)", bool_node->value ? "true" : "false");
    return output;
}

/*
 * Function: zenit_node_bool_free
 *  Frees the memory of a <struct ZenitBoolNode> object
 *
 * Parameters:
 *  bool_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_bool_free(struct ZenitBoolNode *bool_node)
{
    if (!bool_node)
        return;

    fl_free(bool_node);
}