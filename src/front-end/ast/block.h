#ifndef ZENIT_AST_BLOCK_H
#define ZENIT_AST_BLOCK_H

#include "node.h"

/*
 * Struct: struct ZenitBlockNode
 *  Represents a block in the source code
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <struct ZenitNode> **statements: Pointers to the block's statements
 */
struct ZenitBlockNode {
    struct ZenitNode base;
    struct ZenitNode **statements;
};

/*
 * Function: zenit_node_block_new
 *  Creates a new AST node that represents a block
 *
 * Parameters:
 *  <struct ZenitSourceLocation> location: Location information of the block
 *
 * Returns:
 *  struct ZenitBlockNode*: Block node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_block_free> function
 */
struct ZenitBlockNode* zenit_node_block_new(struct ZenitSourceLocation location);

/*
 * Function: zenit_node_block_uid
 *  Returns a UID for the block node
 *
 * Parameters:
 *  <struct ZenitBlockNode> *block_node: Block node object
 *
 * Returns:
 *  char*: UID of the block node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_block_uid(struct ZenitBlockNode *block_node);

/*
 * Function: zenit_node_block_add_statement
 *  Adds a new statement to the block node
 *
 * Parameters:
 *  <struct ZenitBlockNode> *block_node: Block node
 *  <struct ZenitNode> *statement: Statement node
 *
 * Returns:
 *  void: This function does not return a value
 *
 * Notes:
 *  The block node takes ownership of the statement node, which means that the caller does not need to
 *  free the memory of the *statement* object.
 */
void zenit_node_block_add_statement(struct ZenitBlockNode *block_node, struct ZenitNode *statement);

/*
 * Function: zenit_node_block_dump
 *  Appends a dump of the block node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitBlockNode> *block_node: Block node to dump to the output
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
 *      output = zenit_node_block_dump(block_node, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_block_dump(struct ZenitBlockNode *block_node, char *output);

/*
 * Function: zenit_node_block_free
 *  Frees the memory used by the block node and its children
 *
 * Parameters:
 *  <struct ZenitBlockNode> *block_node: Block node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_block_free(struct ZenitBlockNode *block_node);

#endif /* ZENIT_AST_BLOCK_H */
