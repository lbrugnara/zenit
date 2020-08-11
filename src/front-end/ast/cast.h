#ifndef ZENIT_AST_CAST_H
#define ZENIT_AST_CAST_H

#include <stdbool.h>
#include "node.h"
#include "types/type.h"

/*
 * Struct: ZenitCastNode
 *  Represents a cast in the source code
 * 
 * Members:
 *  <ZenitNode> base: Basic information of the node object
 *  <ZenitTypeNode> *type_decl: The type declaration node associated to the cast expression
 *  <ZenitNode> *expression: Pointer to an AST node that represents the expression being casted
 *  <bool> implicit: Determines if the cast is explicit in the source code or implicit (like an upcast)
 */
typedef struct ZenitCastNode {
    ZenitNode base;
    ZenitTypeNode *type_decl;
    ZenitNode *expression;
    bool implicit;
} ZenitCastNode;

/*
 * Function: zenit_cast_node_new
 *  Creates a new AST node that represents a cast expression
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the cast expression
 *  <ZenitNode> *expression: A node that represents the expression being casted
 *  <bool> implicit: Determines the type of cast
 *
 * Returns:
 *  ZenitCastNode*: Cast node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_cast_node_free> function
 */
ZenitCastNode* zenit_cast_node_new(ZenitSourceLocation location, ZenitNode *expression, bool implicit);

/*
 * Function: zenit_cast_node_uid
 *  Returns a UID for the cast node
 *
 * Parameters:
 *  <ZenitCastNode> *cast: Cast node
 *
 * Returns:
 *  char*: UID of the cast node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_cast_node_uid(ZenitCastNode *cast);

/*
 * Function: zenit_cast_node_dump
 *  Appends a dump of the cast node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitCastNode> *cast: Cast node to dump to the output
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
 *      output = zenit_cast_node_dump(cast, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_cast_node_dump(ZenitCastNode *cast, char *output);

/*
 * Function: zenit_cast_node_free
 *  Frees the memory used by the cast node
 *
 * Parameters:
 *  <ZenitCastNode> *cast: Cast node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_cast_node_free(ZenitCastNode *cast_node);

#endif /* ZENIT_AST_CAST_H */
