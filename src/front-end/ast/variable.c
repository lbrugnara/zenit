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

    return fl_cstring_vdup("%%L%u:C%u_var{%s}", variable->base.location.line, variable->base.location.col, variable->name);
}

char* zenit_node_variable_dump(struct ZenitVariableNode *variable, char *output)
{
    fl_cstring_vappend(&output, "(var %s ", variable->name);

    if (variable->type_decl != NULL)
    {
        output = zenit_node_dump((struct ZenitNode*) variable->type_decl, output);
        fl_cstring_append(&output, " ");
    }

    output = zenit_node_dump((struct ZenitNode*) variable->rvalue, output);

    struct ZenitAttributeNode **attrs = zenit_attribute_node_map_values(&variable->attributes);
    size_t length = fl_array_length(attrs);
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
