
#include <fllib/Cstring.h>
#include "if.h"

ZenitIfNode* zenit_node_if_new(ZenitSourceLocation location, ZenitNode *condition, ZenitNode *then_branch, ZenitNode *else_branch)
{
    ZenitIfNode *if_node = fl_malloc(sizeof(ZenitIfNode));
    if_node->base.nodekind = ZENIT_NODE_IF;
    if_node->base.location = location;
    if_node->condition = condition;
    if_node->then_branch = then_branch;
    if_node->else_branch = else_branch;

    return if_node;
}

char* zenit_node_if_uid(ZenitIfNode *if_node)
{
    if (!if_node)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_if", if_node->base.location.line, if_node->base.location.col);
}

char* zenit_node_if_dump(ZenitIfNode *if_node, char *output)
{
    fl_cstring_append(&output, "(if ");
    output = zenit_node_dump(if_node->condition, output);
    fl_cstring_append(&output, " ");
    output = zenit_node_dump(if_node->then_branch, output);

    if (if_node->else_branch)
    {
        fl_cstring_append(&output, " ");
        output = zenit_node_dump(if_node->else_branch, output);
    }

    fl_cstring_append(&output, ")");

    return output;
}

/*
 * Function: zenit_node_if_free
 *  Frees the memory of a <ZenitIfNode> object
 *
 * Parameters:
 *  if_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_if_free(ZenitIfNode *if_node)
{
    if (!if_node)
        return;

    if (if_node->condition)
        zenit_node_free(if_node->condition);

    if (if_node->then_branch)
        zenit_node_free(if_node->then_branch);

    if (if_node->else_branch)
        zenit_node_free(if_node->else_branch);

    fl_free(if_node);
}
