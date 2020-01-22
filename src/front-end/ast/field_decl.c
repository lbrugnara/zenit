#include <fllib.h>
#include "field_decl.h"

struct ZenitFieldDeclNode* zenit_node_field_decl_new(struct ZenitSourceLocation location, char *name)
{
    struct ZenitFieldDeclNode *field_node = fl_malloc(sizeof(struct ZenitFieldDeclNode));
    field_node->base.nodekind = ZENIT_NODE_FIELD_DECL;
    field_node->base.location = location;
    field_node->name = name;

    return field_node;
}

char* zenit_node_field_decl_uid(struct ZenitFieldDeclNode *field)
{
    if (!field)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_field_decl_%s", field->base.location.line, field->base.location.col, field->name);
}

char* zenit_node_field_decl_dump(struct ZenitFieldDeclNode *field, char *output)
{
    fl_cstring_vappend(&output, "(field %s ", field->name);
    
    output = zenit_node_dump((struct ZenitNode*) field->type_decl, output);
    
    fl_cstring_append(&output, ")");

    return output;
}

/*
 * Function: zenit_node_field_decl_free
 *  Frees the memory of a <struct ZenitFieldDeclNode> object
 *
 * Parameters:
 *  field_node - Node object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_field_decl_free(struct ZenitFieldDeclNode *field_node)
{
    if (!field_node)
        return;

    if (field_node->name)
        fl_cstring_free(field_node->name);

    if (field_node->type_decl)
        zenit_node_free((struct ZenitNode*) field_node->type_decl);

    fl_free(field_node);
}
