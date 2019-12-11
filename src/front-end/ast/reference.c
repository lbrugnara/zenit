#include <fllib.h>
#include "reference.h"
#include "../types/reference.h"

struct ZenitReferenceNode* zenit_node_reference_new(struct ZenitSourceLocation location, struct ZenitNode *expression)
{
    struct ZenitReferenceNode *ref_node = fl_malloc(sizeof(struct ZenitReferenceNode));
    ref_node->base.type = ZENIT_NODE_REFERENCE;
    ref_node->base.location = location;
    ref_node->expression = expression;

    return ref_node;
}

char* zenit_node_reference_uid(struct ZenitReferenceNode *reference)
{
    if (!reference)
        return NULL;

    char *expr_id = zenit_node_uid(reference->expression);

    char *id = fl_cstring_vdup("%%L%u_C%u_reference[e:%s]", reference->base.location.line, reference->base.location.col, expr_id != NULL ? expr_id : "<missing>");

    if (expr_id != NULL)
        fl_cstring_free(expr_id);

    return id;
}

/*
 * Function: zenit_node_reference_free
 *  Releases the memory of the <struct ZenitReferenceNode> object
 *
 * Parameters:
 *  ref_node - Object to be freed.
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_reference_free(struct ZenitReferenceNode *ref_node)
{
    if (!ref_node)
        return;

    zenit_node_free(ref_node->expression);

    fl_free(ref_node);
}
