#ifndef ZENIT_AST_STRUCT_DECL_H
#define ZENIT_AST_STRUCT_DECL_H

#include "node.h"
#include "attribute.h"
#include "attribute_map.h"

/*
 * Struct: struct ZenitStructDeclNode
 *  Represents a struct declaration node
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <char> *name: The struct name
 *  <struct ZenitNode> **members: Pointers to the struct members
 *  <struct ZenitAttributeNodeMap> attributes: If present, a list of all the struct attributes
 * 
 */
struct ZenitStructDeclNode {
    struct ZenitNode base;
    char *name;
    struct ZenitNode **members;
    struct ZenitAttributeNodeMap attributes;
};

/*
 * Function: zenit_node_struct_decl_new
 *  Creates a new AST node that represents a struct declaration
 *
 * Parameters:
 *  <struct ZenitSourceLocation> location: Location information about the struct declaration
 *  <char> *name: The struct name
 *
 * Returns:
 *  struct ZenitStructDeclNode*: Struct declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_struct_decl_free> function
 */
struct ZenitStructDeclNode* zenit_node_struct_decl_new(struct ZenitSourceLocation location, char *name);

/*
 * Function: zenit_node_struct_decl_uid
 *  Returns a UID for the struct declaration node
 *
 * Parameters:
 *  <struct ZenitStructDeclNode> *struct_decl: Struct declaration node
 *
 * Returns:
 *  char*: UID of the struct declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_struct_decl_uid(struct ZenitStructDeclNode *struct_node);

/*
 * Function: zenit_node_struct_decl_dump
 *  Appends a dump of the struct declaration node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitStructDeclNode> *struct_decl: Struct declaration node to dump to the output
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
 *      output = zenit_node_struct_decl_dump(struct declaration, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_struct_decl_dump(struct ZenitStructDeclNode *struct_node, char *output);

/*
 * Function: zenit_node_struct_decl_free
 *  Frees the memory used by the struct declaration node
 *
 * Parameters:
 *  <struct ZenitStructDeclNode> *struct_decl: Struct declaration node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_struct_decl_free(struct ZenitStructDeclNode *struct_node);

#endif /* ZENIT_AST_STRUCT_DECL_H */
