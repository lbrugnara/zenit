#include <fllib.h>
#include "array.h"

struct ZenitArrayNode* zenit_node_array_new(struct ZenitSourceLocation location)
{
    struct ZenitArrayNode *node = fl_malloc(sizeof(struct ZenitArrayNode));
    node->base.type = ZENIT_NODE_ARRAY_INIT;
    node->base.location = location;
    node->elements = fl_array_new(sizeof(struct ZenitNode*), 0);

    // Array type information: The type is NONE by default and is
    // updated accordingly in the inference pass based on its content
    node->base.typeinfo.type = ZENIT_TYPE_NONE;
    node->base.typeinfo.name = NULL;
    node->base.typeinfo.elements = 0;
    node->base.typeinfo.is_array = true;

    return node;
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

    fl_array_free_each_pointer(array->elements, (FlArrayFreeElementFunc)zenit_node_free);

    fl_free(array);
}
