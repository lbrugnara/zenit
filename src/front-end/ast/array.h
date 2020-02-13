#ifndef ZENIT_AST_ARRAY_H
#define ZENIT_AST_ARRAY_H

#include "node.h"

/*
 * Struct: struct ZenitArrayNode
 *  Represents an array literal initializer in the source code
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <struct ZenitNode> **elements: Pointers to the array's elements
 */
struct ZenitArrayNode {
    struct ZenitNode base;
    struct ZenitNode **elements;
};

/*
 * Function: zenit_node_array_new
 *  Creates a new AST node that represents an array literal
 *
 * Parameters:
 *  <struct ZenitSourceLocation> location: Location information of the array literal
 *
 * Returns:
 *  struct ZenitArrayNode*: Array node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_array_free> function
 */
struct ZenitArrayNode* zenit_node_array_new(struct ZenitSourceLocation location);

/*
 * Function: zenit_node_array_uid
 *  Returns a UID for the array node
 *
 * Parameters:
 *  <struct ZenitArrayNode> *array: Array node
 *
 * Returns:
 *  char*: UID of the array node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_array_uid(struct ZenitArrayNode *array);

/*
 * Function: zenit_node_array_add_child
 *  Adds an AST node to the set of children of the array node
 *
 * Parameters:
 *  <struct ZenitArrayNode> *array: Array node
 *  <struct ZenitNode> *element: Child node
 *
 * Returns:
 *  void: This function does not return a value
 *
 * Notes:
 *  The array node takes ownership of the element node, which means that the caller does not need to
 *  free the memory of the *element* object.
 */
void zenit_node_array_add_child(struct ZenitArrayNode *array, struct ZenitNode *element);

/*
 * Function: zenit_node_array_dump
 *  Appends a dump of the array node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitArrayNode> *array: Array node to dump to the output
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
 *      output = zenit_node_array_dump(array, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_array_dump(struct ZenitArrayNode *array, char *output);

/*
 * Function: zenit_node_array_free
 *  Frees the memory used by the array node and its children
 *
 * Parameters:
 *  <struct ZenitArrayNode> *array: Array node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_array_free(struct ZenitArrayNode *array);

#endif /* ZENIT_AST_ARRAY_H */
