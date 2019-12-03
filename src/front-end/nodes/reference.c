#include <fllib.h>
#include "reference.h"

struct ZenitUnaryRefNode* zenit_node_unary_ref_new(struct ZenitSourceLocation location, struct ZenitNode *expression)
{
    struct ZenitUnaryRefNode *ref_node = fl_malloc(sizeof(struct ZenitUnaryRefNode));
    ref_node->base.type = ZENIT_NODE_UNARY_REF;
    ref_node->base.location = location;
    ref_node->expression = expression;

    return ref_node;
}

/*
 * Function: zenit_node_unary_ref_free
 *  Releases the memory of the <struct ZenitUnaryRefNode> object
 *
 * Parameters:
 *  ref_node - Object to be freed.
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_unary_ref_free(struct ZenitUnaryRefNode *ref_node)
{
    if (!ref_node)
        return;

    zenit_node_free(ref_node->expression);

    fl_free(ref_node);
}
