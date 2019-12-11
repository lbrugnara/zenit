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

char* zenit_node_variable_uid(struct ZenitVariableNode *variable)
{
    if (!variable)
        return NULL;

    char *rhs_id = zenit_node_uid(variable->rvalue);

    char *id = fl_cstring_vdup("%%L%u_C%u_variable[lhs:%s][rhs:%s]", 
        variable->base.location.line, variable->base.location.col, variable->name, rhs_id != NULL ? rhs_id : "<missing>");

    if (rhs_id != NULL)
        fl_cstring_free(rhs_id);

    return id;
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

    if (var_node->type_decl)
        zenit_node_free((struct ZenitNode*) var_node->type_decl);

    zenit_attribute_node_map_free(&var_node->attributes);

    fl_free(var_node);
}
