
#include <fllib/Cstring.h>
#include "field-decl.h"

ZenitFieldDeclNode* zenit_field_decl_node_new(ZenitSourceLocation location, char *name)
{
    ZenitFieldDeclNode *field_node = fl_malloc(sizeof(ZenitFieldDeclNode));
    field_node->base.nodekind = ZENIT_NODE_FIELD_DECL;
    field_node->base.location = location;
    field_node->name = name;

    return field_node;
}

char* zenit_field_decl_node_uid(ZenitFieldDeclNode *field)
{
    if (!field)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_field_decl_%s", field->base.location.line, field->base.location.col, field->name);
}

char* zenit_field_decl_node_dump(ZenitFieldDeclNode *field, char *output)
{
    fl_cstring_vappend(&output, "(field %s ", field->name);
    
    output = zenit_node_dump((ZenitNode*) field->type_decl, output);
    
    fl_cstring_append(&output, ")");

    return output;
}

/*
 * Function: zenit_field_decl_node_free
 *  Frees the memory of a <ZenitFieldDeclNode> object
 *
 * Parameters:
 *  field_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_field_decl_node_free(ZenitFieldDeclNode *field_node)
{
    if (!field_node)
        return;

    if (field_node->name)
        fl_cstring_free(field_node->name);

    if (field_node->type_decl)
        zenit_node_free((ZenitNode*) field_node->type_decl);

    fl_free(field_node);
}
