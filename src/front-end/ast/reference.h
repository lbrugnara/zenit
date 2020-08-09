#ifndef ZENIT_AST_REFERENCE_H
#define ZENIT_AST_REFERENCE_H

#include "node.h"

/*
 * Struct: ZenitReferenceNode
 *  Represents taking an expression's address (reference operator)
 * 
 * Members:
 *  <ZenitNode> base: Basic information of the node object
 *  <ZenitNode> *expression: The expression to take its address from
 */
typedef struct ZenitReferenceNode {
    ZenitNode base;
    ZenitNode *expression;
} ZenitReferenceNode;

/*
 * Function: zenit_node_reference_new
 *  Creates a new AST node that represents a reference expression
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the reference expression
 *  <ZenitNode> *expression: A node that represents the expression being referenced
 *
 * Returns:
 *  ZenitReferenceNode*: Reference node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_reference_free> function
 */
ZenitReferenceNode* zenit_node_reference_new(ZenitSourceLocation location, ZenitNode *expression);

/*
 * Function: zenit_node_reference_uid
 *  Returns a UID for the reference node
 *
 * Parameters:
 *  <ZenitReferenceNode> *reference: Reference node
 *
 * Returns:
 *  char*: UID of the reference node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_reference_uid(ZenitReferenceNode *reference);

/*
 * Function: zenit_node_reference_dump
 *  Appends a dump of the reference node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitReferenceNode> *reference: Reference node to dump to the output
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
 *      output = zenit_node_reference_dump(reference, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_reference_dump(ZenitReferenceNode *reference, char *output);

/*
 * Function: zenit_node_reference_free
 *  Frees the memory used by the reference node
 *
 * Parameters:
 *  <ZenitReferenceNode> *reference: Reference node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_reference_free(ZenitReferenceNode *ref_node);

#endif /* ZENIT_AST_REFERENCE_H */
