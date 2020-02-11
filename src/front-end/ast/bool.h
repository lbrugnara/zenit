#ifndef ZENIT_AST_BOOL_H
#define ZENIT_AST_BOOL_H

#include <stdint.h>
#include "node.h"
#include "../types/bool.h"

/*
 * Struct: struct ZenitBoolNode
 *  An AST node that represents a boolean literal in the source code
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <enum ZenitBoolTypeSize> size: The size in bits of the boolean literal
 *  <bool> value: The actual value of the boolean literal
 * 
 */
struct ZenitBoolNode {
    struct ZenitNode base;
    bool value;
};

/*
 * Function: zenit_node_bool_new
 *  Creates a new AST node that represents a boolean literal
 *
 * Parameters:
 *  <struct ZenitSourceLocation> location: Location information about the boolean literal
 *  <bool> value: The actual value of the boolean literal
 *
 * Returns:
 *  struct ZenitBoolNode*: Bool node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_bool_free> function
 */
struct ZenitBoolNode* zenit_node_bool_new(struct ZenitSourceLocation location, bool value);

/*
 * Function: zenit_node_bool_uid
 *  Returns a UID for the bool node
 *
 * Parameters:
 *  <struct ZenitBoolNode> *bool_node: Bool node
 *
 * Returns:
 *  char*: UID of the bool node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_bool_uid(struct ZenitBoolNode *bool_node);

/*
 * Function: zenit_node_bool_dump
 *  Appends a dump of the bool node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitBoolNode> *bool_node: Bool node to dump to the output
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
 *      output = zenit_node_bool_dump(bool, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_bool_dump(struct ZenitBoolNode *bool_node, char *output);

/*
 * Function: zenit_node_bool_free
 *  Frees the memory used by the bool node
 *
 * Parameters:
 *  <struct ZenitBoolNode> *bool_node: Bool node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_bool_free(struct ZenitBoolNode *bool_node);

#endif /* ZENIT_AST_BOOL_H */
