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

char* zenit_node_array_uid(struct ZenitArrayNode *array)
{
    if (!array)
        return NULL;

    return fl_cstring_vdup(
        "%%L%u:C%u_array[e:%zu]", 
        array->base.location.line, 
        array->base.location.col, 
        fl_array_length(array->elements)
    );
}

/*
 f Function: zenit_node_array_free
 *  Releases the memory of an array node object
 *
 * Parameters:
 *  <struct ZenitArrayNode> *array: Array node object
 *
 * Returns:
 *  <void>: This function does not return a value
 */
void zenit_node_array_free(struct ZenitArrayNode *array)
{
    if (!array)
        return;

    fl_array_free_each_pointer(array->elements, (FlArrayFreeElementFunc) zenit_node_free);

    fl_free(array);
}
