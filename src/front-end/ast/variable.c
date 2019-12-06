#include <fllib.h>
#include "variable.h"

struct ZenitVariableNode* zenit_node_variable_new(struct ZenitSourceLocation location, char *name)
{
    struct ZenitVariableNode *var_node = fl_malloc(sizeof(struct ZenitVariableNode));
    var_node->base.type = ZENIT_NODE_VARIABLE;
    var_node->base.location = location;
    var_node->name = name;

    return var_node;
}

/*
 * Function: zenit_node_variable_free
 *  Frees the memory of a <struct ZenitVariableNode> object
 *
 * Parameters:
 *  var_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_variable_free(struct ZenitVariableNode *var_node)
{
    if (!var_node)
        return;

    if (var_node->name)
        fl_cstring_free(var_node->name);

    if (var_node->rvalue)
        zenit_node_free(var_node->rvalue);

    zenit_attribute_node_map_free(&var_node->attributes);

    fl_free(var_node);
}
