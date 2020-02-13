#ifndef ZENIT_AST_IF_H
#define ZENIT_AST_IF_H

#include "node.h"

/*
 * Struct: struct ZenitIfNode
 *  Represents an if statement
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <struct ZenitNode> *condition: The node that represents the conditional check of the if
 *  <struct ZenitNode> *then_branch: The node that represents the branch to take if the if condition is true
 *  <struct ZenitNode> *else_branch: The node that represents the branch to take if the if condition is false
 */
struct ZenitIfNode {
    struct ZenitNode base;
    struct ZenitNode *condition;
    struct ZenitNode *then_branch;
    struct ZenitNode *else_branch;
};

/*
 * Function: zenit_node_if_new
 *  Creates a new AST node that represents an if statement
 *
 * Parameters:
 *  <struct ZenitSourceLocation> location: Location information about the if statement
 *  <struct ZenitNode> *condition: The node that represents the conditional check of the if
 *  <struct ZenitNode> *then_branch: The node that represents the branch to take if the if condition is true
 *  <struct ZenitNode> *else_branch: The node that represents the branch to take if the if condition is false
 *
 * Returns:
 *  struct ZenitIfNode*: If statement node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_if_free> function
 */
struct ZenitIfNode* zenit_node_if_new(struct ZenitSourceLocation location, struct ZenitNode *condition, struct ZenitNode *then_branch, struct ZenitNode *else_branch);

/*
 * Function: zenit_node_if_uid
 *  Returns a UID for the if statement node
 *
 * Parameters:
 *  <struct ZenitIfNode> *if_node: If statement node
 *
 * Returns:
 *  char*: UID of the if statement node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_if_uid(struct ZenitIfNode *if_node);

/*
 * Function: zenit_node_if_dump
 *  Appends a dump of the if statement node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitIfNode> *if_node: If statement node to dump to the output
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
 *      output = zenit_node_if_dump(if_node, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_if_dump(struct ZenitIfNode *if_node, char *output);

/*
 * Function: zenit_node_if_free
 *  Frees the memory used by the if statement node
 *
 * Parameters:
 *  <struct ZenitIfNode> *if_node: If statement node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_if_free(struct ZenitIfNode *if_node);

#endif /* ZENIT_AST_IF_H */
