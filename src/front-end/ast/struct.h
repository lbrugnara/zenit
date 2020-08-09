#ifndef ZENIT_AST_STRUCT_H
#define ZENIT_AST_STRUCT_H

#include "node.h"

/*
 * Struct: ZenitStructNode
 *  An AST node that represents a struct literal expression
 * 
 * Members:
 *  <ZenitNode> base: Basic information of the node object
 *  <char> *name: The struct name or <NULL> for unnamed structs
 *  <ZenitNode> **members: Array of pointers to the struct members
 * 
 */
typedef struct ZenitStructNode {
    ZenitNode base;
    char *name;
    ZenitNode **members;
} ZenitStructNode;

/*
 * Function: zenit_node_struct_new
 *  Creates a new AST node that represents a struct literal
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the struct literal
 *  <char> *name: Name of the struct or <NULL> for unnamed structs
 *
 * Returns:
 *  ZenitStructNode*: Struct node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_struct_free> function
 */
ZenitStructNode* zenit_node_struct_new(ZenitSourceLocation location, char *name);

/*
 * Function: zenit_node_struct_uid
 *  Returns a UID for the struct node
 *
 * Parameters:
 *  <ZenitStructNode> *struct_node: Struct node
 *
 * Returns:
 *  char*: UID of the struct node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_struct_uid(ZenitStructNode *struct_node);

/*
 * Function: zenit_node_struct_dump
 *  Appends a dump of the struct node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitStructNode> *struct_node: Struct node to dump to the output
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
 *      output = zenit_node_struct_dump(struct, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_struct_dump(ZenitStructNode *struct_node, char *output);

/*
 * Function: zenit_node_struct_free
 *  Frees the memory used by the struct node
 *
 * Parameters:
 *  <ZenitStructNode> *struct_node: Struct node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_struct_free(ZenitStructNode *struct_node);

#endif /* ZENIT_AST_STRUCT_H */
