
#include <fllib/Cstring.h>
#include "field.h"

ZenitFieldNode* zenit_field_node_new(ZenitSourceLocation location, char *name)
{
    ZenitFieldNode *field_node = fl_malloc(sizeof(ZenitFieldNode));
    field_node->base.nodekind = ZENIT_NODE_FIELD;
    field_node->base.location = location;
    field_node->name = name;

    return field_node;
}

char* zenit_field_node_uid(ZenitFieldNode *field)
{
    if (!field)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_field_%s", field->base.location.line, field->base.location.col, field->name);
}

char* zenit_field_node_dump(ZenitFieldNode *field, char *output)
{
    fl_cstring_vappend(&output, "(%s ", field->name);
    
    output = zenit_node_dump((ZenitNode*) field->value, output);
    
    fl_cstring_append(&output, ")");

    return output;
}

/*
 * Function: zenit_field_node_free
 *  Frees the memory of a <ZenitFieldNode> object
 *
 * Parameters:
 *  field_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_field_node_free(ZenitFieldNode *field_node)
{
    if (!field_node)
        return;

    if (field_node->name)
        fl_cstring_free(field_node->name);

    if (field_node->value)
        zenit_node_free((ZenitNode*) field_node->value);

    fl_free(field_node);
}
