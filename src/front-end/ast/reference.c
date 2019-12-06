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
