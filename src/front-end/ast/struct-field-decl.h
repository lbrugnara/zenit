#ifndef ZENIT_AST_FIELD_DECL_H
#define ZENIT_AST_FIELD_DECL_H

#include "node.h"
#include "types/type.h"

/*
 * Struct: ZenitStructFieldDeclNode
 *  Represents a field declaration node
 * 
 * Members:
 *  <ZenitNode> base: Basic information of the node object
 *  <char> *name: The field name
 *  <ZenitTypeNode> *type_decl: The field's type information
 *  <ZenitNode> *owner: The field's parent node
 * 
 */
typedef struct ZenitStructFieldDeclNode {
    ZenitNode base;
    char *name;
    ZenitTypeNode *type_decl;
    ZenitNode *owner;
} ZenitStructFieldDeclNode;

/*
 * Function: zenit_struct_field_decl_node_new
 *  Creates a new AST node that represents a field declaration
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the field declaration
 *  <char> *name: The name of the field
 *
 * Returns:
 *  ZenitStructFieldDeclNode*: Field declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_struct_field_decl_node_free> function
 */
ZenitStructFieldDeclNode* zenit_struct_field_decl_node_new(ZenitSourceLocation location, char *name);

/*
 * Function: zenit_struct_field_decl_node_uid
 *  Returns a UID for the field declaration node
 *
 * Parameters:
 *  <ZenitStructFieldDeclNode> *field_decl: Field declaration node
 *
 * Returns:
 *  char*: UID of the field declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_struct_field_decl_node_uid(ZenitStructFieldDeclNode *field_decl);

/*
 * Function: zenit_struct_field_decl_node_dump
 *  Appends a dump of the field declaration node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitStructFieldDeclNode> *field_decl: Field declaration node to dump to the output
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
 *      output = zenit_struct_field_decl_node_dump(field_decl, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_struct_field_decl_node_dump(ZenitStructFieldDeclNode *field_decl, char *output);

/*
 * Function: zenit_struct_field_decl_node_free
 *  Frees the memory used by the field declaration node
 *
 * Parameters:
 *  <ZenitStructFieldDeclNode> *field_decl: Field declaration node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_struct_field_decl_node_free(ZenitStructFieldDeclNode *field_decl);

#endif /* ZENIT_AST_FIELD_DECL_H */
