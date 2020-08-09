
#include <fllib/Cstring.h>
#include "variable.h"

ZenitVariableNode* zenit_node_variable_new(ZenitSourceLocation location, char *name)
{
    ZenitVariableNode *var_node = fl_malloc(sizeof(ZenitVariableNode));
    var_node->base.nodekind = ZENIT_NODE_VARIABLE;
    var_node->base.location = location;
    var_node->name = name;

    return var_node;
}

char* zenit_node_variable_uid(ZenitVariableNode *variable)
{
    if (!variable)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_var_%s", variable->base.location.line, variable->base.location.col, variable->name);
}

char* zenit_node_variable_dump(ZenitVariableNode *variable, char *output)
{
    fl_cstring_vappend(&output, "(var %s ", variable->name);

    if (variable->type_decl != NULL)
    {
        output = zenit_node_dump((ZenitNode*) variable->type_decl, output);
        fl_cstring_append(&output, " ");
    }

    output = zenit_node_dump((ZenitNode*) variable->rvalue, output);

    ZenitAttributeNode **attrs = zenit_attribute_node_map_values(variable->attributes);
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
 *  Frees the memory of a <ZenitVariableNode> object
 *
 * Parameters:
 *  var_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_variable_free(ZenitVariableNode *var_node)
{
    if (!var_node)
        return;

    if (var_node->name)
        fl_cstring_free(var_node->name);

    if (var_node->rvalue)
        zenit_node_free(var_node->rvalue);

    if (var_node->type_decl)
        zenit_node_free((ZenitNode*) var_node->type_decl);

    zenit_attribute_node_map_free(var_node->attributes);

    fl_free(var_node);
}
