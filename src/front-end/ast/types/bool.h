#ifndef ZENIT_AST_TYPE_BOOL_H
#define ZENIT_AST_TYPE_BOOL_H

#include "bool.h"
#include "../../types/bool.h"

/*
 * Struct: struct ZenitBoolTypeNode
 *  Represents a boolean type declaration in the source code
 * 
 * Members:
 *  <struct ZenitTypeNode> base: Basic information of the type node object
 */
struct ZenitBoolTypeNode {
    struct ZenitTypeNode base;
};

/*
 * Function: zenit_node_type_bool_new
 *  Creates a new AST node that represents a boolean type declaration
 *
 * Parameters:
 *  <struct ZenitSourceLocation> location: Location information about the boolean type declaration
 * 
 * Returns:
 *  struct ZenitBoolTypeNode*: Boolean type declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_type_bool_free> function
 */
struct ZenitBoolTypeNode* zenit_node_type_bool_new(struct ZenitSourceLocation location);

/*
 * Function: zenit_node_type_bool_uid
 *  Returns a UID for the boolean type declaration node
 *
 * Parameters:
 *  <struct ZenitBoolTypeNode> *bool_type_node: Boolean type declaration node
 *
 * Returns:
 *  char*: UID of the boolean type declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_type_bool_uid(struct ZenitBoolTypeNode *bool_type_node);

/*
 * Function: zenit_node_type_bool_dump
 *  Appends a dump of the boolean type declaration node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitBoolTypeNode> *type_node: Boolean type declaration node to dump to the output
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
 *      output = zenit_node_type_bool_dump(bool_type, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_type_bool_dump(struct ZenitBoolTypeNode *type_node, char *output);

/*
 * Function: zenit_node_type_bool_to_string
 *  Returns the string representation of the type declaration
 *
 * Parameters:
 *  <struct ZenitBoolTypeNode> *bool_type_node: The type node object
 *
 * Returns:
 *  char*: The string representation of the type declaration
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_type_bool_to_string(struct ZenitBoolTypeNode *bool_type_node);

/*
 * Function: zenit_node_type_bool_free
 *  Frees the memory used by the boolean type declaration node
 *
 * Parameters:
 *  <struct ZenitBoolTypeNode> *bool_type_node: Boolean type declaration node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_type_bool_free(struct ZenitBoolTypeNode *bool_type_node);

#endif /* ZENIT_AST_TYPE_BOOL_H */
