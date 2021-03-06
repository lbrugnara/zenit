#ifndef ZENIT_AST_IDENTIFIER_H
#define ZENIT_AST_IDENTIFIER_H

#include "node.h"

/*
 * Struct: ZenitIdentifierNode
 *  Represents a reference to an identifier in the source code
 * 
 * Members:
 *  <ZenitNode> base: Basic information of the node object
 *  <char> *name: The identifier name
 */
typedef struct ZenitIdentifierNode {
    ZenitNode base;
    char *name;
} ZenitIdentifierNode;

/*
 * Function: zenit_identifier_node_new
 *  Creates a new AST node that represents an identifier
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the identifier
 *  <char> *name: The identifier name
 *
 * Returns:
 *  ZenitIdentifierNode*: Identifier node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_identifier_node_free> function
 */
ZenitIdentifierNode* zenit_identifier_node_new(ZenitSourceLocation location, char *name);

/*
 * Function: zenit_identifier_node_uid
 *  Returns a UID for the identifier node
 *
 * Parameters:
 *  <ZenitIdentifierNode> *identifier: Identifier node
 *
 * Returns:
 *  char*: UID of the identifier node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_identifier_node_uid(ZenitIdentifierNode *identifier);

/*
 * Function: zenit_identifier_node_dump
 *  Appends a dump of the identifier node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitIdentifierNode> *identifier: Identifier node to dump to the output
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
 *      output = zenit_identifier_node_dump(identifier, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_identifier_node_dump(ZenitIdentifierNode *identifier, char *output);

/*
 * Function: zenit_identifier_node_free
 *  Frees the memory used by the identifier node
 *
 * Parameters:
 *  <ZenitIdentifierNode> *identifier: Identifier node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_identifier_node_free(ZenitIdentifierNode *identifier_node);

#endif /* ZENIT_AST_IDENTIFIER_H */
