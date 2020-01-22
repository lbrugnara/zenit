#ifndef ZENIT_AST_VARIABLE_H
#define ZENIT_AST_VARIABLE_H

#include "node.h"
#include "attribute.h"
#include "attribute_map.h"

/*
 * Struct: struct ZenitVariableNode
 *  Represents a variable declaration node
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <char> *name: The variable name
 *  <struct ZenitTypeNode> *type_decl: If present in the declaration, the variable's type
 *  <struct ZenitNode> *rvalue: The right-hand side expression that initializes the variable
 *  <struct ZenitAttributeNodeMap> attributes: If present, a list of all the variable's attributes
 * 
 */
struct ZenitVariableNode {
    struct ZenitNode base;
    char *name;
    struct ZenitTypeNode *type_decl;
    struct ZenitNode *rvalue;
    struct ZenitAttributeNodeMap attributes;
};

/*
 * Function: zenit_node_variable_new
 *  Creates a new AST node that represents a variable declaration
 *
 * Parameters:
 *  <struct ZenitSourceLocation> location: Location information about the variable declaration
 *  <char> *name: The name of the variable
 *
 * Returns:
 *  struct ZenitVariableNode*: Variable declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_variable_free> function
 */
struct ZenitVariableNode* zenit_node_variable_new(struct ZenitSourceLocation location, char *name);

/*
 * Function: zenit_node_variable_uid
 *  Returns a UID for the variable declaration node
 *
 * Parameters:
 *  <struct ZenitVariableNode> *variable: Variable declaration node
 *
 * Returns:
 *  char*: UID of the variable declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_variable_uid(struct ZenitVariableNode *variable);

/*
 * Function: zenit_node_variable_dump
 *  Appends a dump of the variable declaration node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitVariableNode> *variable: Variable declaration node to dump to the output
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
 *      output = zenit_node_variable_dump(variable, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_variable_dump(struct ZenitVariableNode *variable, char *output);

/*
 * Function: zenit_node_variable_free
 *  Frees the memory used by the variable declaration node
 *
 * Parameters:
 *  <struct ZenitVariableNode> *variable: Variable declaration node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_variable_free(struct ZenitVariableNode *var_node);

#endif /* ZENIT_AST_VARIABLE_H */
