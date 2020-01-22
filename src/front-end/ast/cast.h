#ifndef ZENIT_AST_CAST_H
#define ZENIT_AST_CAST_H

#include <stdbool.h>
#include "node.h"
#include "../types/type.h"

/*
 * Struct: struct ZenitCastNode
 *  Represents a cast in the source code
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <struct ZenitTypeNode> *type_decl: The type declaration node associated to the cast expression
 *  <struct ZenitNode> *expression: Pointer to an AST node that represents the expression being casted
 *  <bool> implicit: Determines if the cast is explicit in the source code or implicit (like an upcast)
 */
struct ZenitCastNode {
    struct ZenitNode base;
    struct ZenitTypeNode *type_decl;
    struct ZenitNode *expression;
    bool implicit;
};

/*
 * Function: zenit_node_cast_new
 *  Creates a new AST node that represents a cast expression
 *
 * Parameters:
 *  <struct ZenitSourceLocation> location: Location information about the cast expression
 *  <struct ZenitNode> *expression: A node that represents the expression being casted
 *  <bool> implicit: Determines the type of cast
 *
 * Returns:
 *  struct ZenitCastNode*: Cast node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_cast_free> function
 */
struct ZenitCastNode* zenit_node_cast_new(struct ZenitSourceLocation location, struct ZenitNode *expression, bool implicit);

/*
 * Function: zenit_node_cast_uid
 *  Returns a UID for the cast node
 *
 * Parameters:
 *  <struct ZenitCastNode> *cast: Cast node
 *
 * Returns:
 *  char*: UID of the cast node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_cast_uid(struct ZenitCastNode *cast);

/*
 * Function: zenit_node_cast_dump
 *  Appends a dump of the cast node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitCastNode> *cast: Cast node to dump to the output
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
 *      output = zenit_node_cast_dump(cast, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_cast_dump(struct ZenitCastNode *cast, char *output);

/*
 * Function: zenit_node_cast_free
 *  Frees the memory used by the cast node
 *
 * Parameters:
 *  <struct ZenitCastNode> *cast: Cast node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_cast_free(struct ZenitCastNode *cast_node);

#endif /* ZENIT_AST_CAST_H */
