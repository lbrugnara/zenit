
#include <fllib/Cstring.h>
#include "reference.h"
#include "../types/reference.h"

ZenitReferenceNode* zenit_reference_node_new(ZenitSourceLocation location, ZenitNode *expression)
{
    ZenitReferenceNode *ref_node = fl_malloc(sizeof(ZenitReferenceNode));
    ref_node->base.nodekind = ZENIT_AST_NODE_REFERENCE;
    ref_node->base.location = location;
    ref_node->expression = expression;

    return ref_node;
}

char* zenit_reference_node_uid(ZenitReferenceNode *reference)
{
    if (!reference)
        return NULL;

    char *expr_id = zenit_node_uid(reference->expression);

    char *id = fl_cstring_vdup("%%L%u:C%u_ref_%s", reference->base.location.line, reference->base.location.col, expr_id != NULL ? expr_id : "<missing>");

    if (expr_id != NULL)
        fl_cstring_free(expr_id);

    return id;
}

char* zenit_reference_node_dump(ZenitReferenceNode *reference, char *output)
{
    fl_cstring_append(&output, "(ref ");

    output = zenit_node_dump(reference->expression, output);

    fl_cstring_append(&output, ")");

    return output;
}

/*
 * Function: zenit_reference_node_free
 *  Releases the memory of the <ZenitReferenceNode> object
 *
 * Parameters:
 *  ref_node - Object to be freed.
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_reference_node_free(ZenitReferenceNode *ref_node)
{
    if (!ref_node)
        return;

    zenit_node_free(ref_node->expression);

    fl_free(ref_node);
}
