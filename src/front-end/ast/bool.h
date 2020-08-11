#ifndef ZENIT_AST_BOOL_H
#define ZENIT_AST_BOOL_H

#include <stdint.h>
#include "node.h"
#include "../types/bool.h"

/*
 * Struct: ZenitBoolNode
 *  An AST node that represents a boolean literal in the source code
 * 
 * Members:
 *  <ZenitNode> base: Basic information of the node object
 *  <enum ZenitBoolTypeSize> size: The size in bits of the boolean literal
 *  <bool> value: The actual value of the boolean literal
 * 
 */
typedef struct ZenitBoolNode {
    ZenitNode base;
    bool value;
} ZenitBoolNode;

/*
 * Function: zenit_bool_node_new
 *  Creates a new AST node that represents a boolean literal
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the boolean literal
 *  <bool> value: The actual value of the boolean literal
 *
 * Returns:
 *  ZenitBoolNode*: Bool node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_bool_node_free> function
 */
ZenitBoolNode* zenit_bool_node_new(ZenitSourceLocation location, bool value);

/*
 * Function: zenit_bool_node_uid
 *  Returns a UID for the bool node
 *
 * Parameters:
 *  <ZenitBoolNode> *bool_node: Bool node
 *
 * Returns:
 *  char*: UID of the bool node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_bool_node_uid(ZenitBoolNode *bool_node);

/*
 * Function: zenit_bool_node_dump
 *  Appends a dump of the bool node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitBoolNode> *bool_node: Bool node to dump to the output
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
 *      output = zenit_bool_node_dump(bool, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_bool_node_dump(ZenitBoolNode *bool_node, char *output);

/*
 * Function: zenit_bool_node_free
 *  Frees the memory used by the bool node
 *
 * Parameters:
 *  <ZenitBoolNode> *bool_node: Bool node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_bool_node_free(ZenitBoolNode *bool_node);

#endif /* ZENIT_AST_BOOL_H */
