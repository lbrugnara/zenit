#ifndef ZENIT_AST_ARRAY_H
#define ZENIT_AST_ARRAY_H

#include "node.h"

/*
 * Struct: ZenitArrayNode
 *  Represents an array literal initializer in the source code
 * 
 * Members:
 *  <ZenitNode> base: Basic information of the node object
 *  <ZenitNode> **elements: Pointers to the array's elements
 */
typedef struct ZenitArrayNode {
    ZenitNode base;
    ZenitNode **elements;
} ZenitArrayNode;

/*
 * Function: zenit_array_node_new
 *  Creates a new AST node that represents an array literal
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information of the array literal
 *
 * Returns:
 *  ZenitArrayNode*: Array node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_array_node_free> function
 */
ZenitArrayNode* zenit_array_node_new(ZenitSourceLocation location);

/*
 * Function: zenit_array_node_uid
 *  Returns a UID for the array node
 *
 * Parameters:
 *  <ZenitArrayNode> *array: Array node
 *
 * Returns:
 *  char*: UID of the array node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_array_node_uid(ZenitArrayNode *array);

/*
 * Function: zenit_array_node_add_child
 *  Adds an AST node to the set of children of the array node
 *
 * Parameters:
 *  <ZenitArrayNode> *array: Array node
 *  <ZenitNode> *element: Child node
 *
 * Returns:
 *  void: This function does not return a value
 *
 * Notes:
 *  The array node takes ownership of the element node, which means that the caller does not need to
 *  free the memory of the *element* object.
 */
void zenit_array_node_add_child(ZenitArrayNode *array, ZenitNode *element);

/*
 * Function: zenit_array_node_dump
 *  Appends a dump of the array node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitArrayNode> *array: Array node to dump to the output
 *  <char> *output: Pointer to a heap allocated string
 *
 * Returns:
 *  char*: Pointer to the output string
 *
 * Notes:
 *  Because the *output* pointer can be modified, this function returns
 *  a pointer to the new location in case the memory is reallocated or
 *  to the old location in case the pointer does not need to be modified. Either
 *  way, it is safe to use the function as:
 *      output = zenit_array_node_dump(array, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_array_node_dump(ZenitArrayNode *array, char *output);

/*
 * Function: zenit_array_node_free
 *  Frees the memory used by the array node and its children
 *
 * Parameters:
 *  <ZenitArrayNode> *array: Array node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_array_node_free(ZenitArrayNode *array);

#endif /* ZENIT_AST_ARRAY_H */
