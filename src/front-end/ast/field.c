
#include <fllib/Cstring.h>
#include "field.h"

struct ZenitFieldNode* zenit_node_field_new(struct ZenitSourceLocation location, char *name)
{
    struct ZenitFieldNode *field_node = fl_malloc(sizeof(struct ZenitFieldNode));
    field_node->base.nodekind = ZENIT_NODE_FIELD;
    field_node->base.location = location;
    field_node->name = name;

    return field_node;
}

char* zenit_node_field_uid(struct ZenitFieldNode *field)
{
    if (!field)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_field_%s", field->base.location.line, field->base.location.col, field->name);
}

char* zenit_node_field_dump(struct ZenitFieldNode *field, char *output)
{
    fl_cstring_vappend(&output, "(%s ", field->name);
    
    output = zenit_node_dump((struct ZenitNode*) field->value, output);
    
    fl_cstring_append(&output, ")");

    return output;
}

/*
 * Function: zenit_node_field_free
 *  Frees the memory of a <struct ZenitFieldNode> object
 *
 * Parameters:
 *  field_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_field_free(struct ZenitFieldNode *field_node)
{
    if (!field_node)
        return;

    if (field_node->name)
        fl_cstring_free(field_node->name);

    if (field_node->value)
        zenit_node_free((struct ZenitNode*) field_node->value);

    fl_free(field_node);
}
