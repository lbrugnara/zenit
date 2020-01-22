#ifndef ZENIT_AST_FIELD_DECL_H
#define ZENIT_AST_FIELD_DECL_H

#include "node.h"

/*
 * Struct: struct ZenitFieldDeclNode
 *  Represents a field declaration node
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <char> *name: The field name
 *  <struct ZenitTypeNode> *type_decl: The field's type information
 *  <struct ZenitNode> *owner: The field's parent node
 * 
 */
struct ZenitFieldDeclNode {
    struct ZenitNode base;
    char *name;
    struct ZenitTypeNode *type_decl;
    struct ZenitNode *owner;
};

/*
 * Function: zenit_node_field_decl_new
 *  Creates a new AST node that represents a field declaration
 *
 * Parameters:
 *  <struct ZenitSourceLocation> location: Location information about the field declaration
 *  <char> *name: The name of the field
 *
 * Returns:
 *  struct ZenitFieldDeclNode*: Field declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_field_decl_free> function
 */
struct ZenitFieldDeclNode* zenit_node_field_decl_new(struct ZenitSourceLocation location, char *name);

/*
 * Function: zenit_node_field_decl_uid
 *  Returns a UID for the field declaration node
 *
 * Parameters:
 *  <struct ZenitFieldDeclNode> *field_decl: Field declaration node
 *
 * Returns:
 *  char*: UID of the field declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_field_decl_uid(struct ZenitFieldDeclNode *field_decl);

/*
 * Function: zenit_node_field_decl_dump
 *  Appends a dump of the field declaration node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitFieldDeclNode> *field_decl: Field declaration node to dump to the output
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
 *      output = zenit_node_field_decl_dump(field_decl, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_field_decl_dump(struct ZenitFieldDeclNode *field_decl, char *output);

/*
 * Function: zenit_node_field_decl_free
 *  Frees the memory used by the field declaration node
 *
 * Parameters:
 *  <struct ZenitFieldDeclNode> *field_decl: Field declaration node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_field_decl_free(struct ZenitFieldDeclNode *field_decl);

#endif /* ZENIT_AST_FIELD_DECL_H */
