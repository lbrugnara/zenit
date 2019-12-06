#include <fllib.h>
#include "array.h"

struct ZenitArrayNode* zenit_node_array_new(struct ZenitSourceLocation location)
{
    struct ZenitArrayNode *node = fl_malloc(sizeof(struct ZenitArrayNode));
    node->base.type = ZENIT_NODE_ARRAY;
    node->base.location = location;
    node->elements = fl_array_new(sizeof(struct ZenitNode*), 0);

    return node;
}

void zenit_node_array_add_child(struct ZenitArrayNode *array, struct ZenitNode *element)
{
    // Add the node to the elements list
    array->elements = fl_array_append(array->elements, &element);
}

/*
 f Function: zenit_node_array_free
 *  Releases the memory of a <struct ZenitArrayNode> object
 *
 * Parameters:
 *  array - <struct ZenitArrayNode> object
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_array_free(struct ZenitArrayNode *array)
{
    if (!array)
        return;

    // No need to free the elements, they belong to other nodes
    fl_array_free(array->elements);

    fl_free(array);
}
