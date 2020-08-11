#ifndef ZENIT_AST_VARIABLE_H
#define ZENIT_AST_VARIABLE_H

#include "node.h"
#include "attribute.h"
#include "attribute-map.h"
#include "types/type.h"

/*
 * Struct: ZenitVariableNode
 *  Represents a variable declaration node
 * 
 * Members:
 *  <ZenitNode> base: Basic information of the node object
 *  <char> *name: The variable name
 *  <ZenitTypeNode> *type_decl: If present in the declaration, the variable's type
 *  <ZenitNode> *rvalue: The right-hand side expression that initializes the variable
 *  <ZenitAttributeNodeMap> *attributes: If present, a list of all the variable's attributes
 * 
 */
typedef struct ZenitVariableNode {
    ZenitNode base;
    char *name;
    ZenitTypeNode *type_decl;
    ZenitNode *rvalue;
    ZenitAttributeNodeMap *attributes;
} ZenitVariableNode;

/*
 * Function: zenit_variable_node_new
 *  Creates a new AST node that represents a variable declaration
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the variable declaration
 *  <char> *name: The name of the variable
 *
 * Returns:
 *  ZenitVariableNode*: Variable declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_variable_node_free> function
 */
ZenitVariableNode* zenit_variable_node_new(ZenitSourceLocation location, char *name);

/*
 * Function: zenit_variable_node_uid
 *  Returns a UID for the variable declaration node
 *
 * Parameters:
 *  <ZenitVariableNode> *variable: Variable declaration node
 *
 * Returns:
 *  char*: UID of the variable declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_variable_node_uid(ZenitVariableNode *variable);

/*
 * Function: zenit_variable_node_dump
 *  Appends a dump of the variable declaration node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitVariableNode> *variable: Variable declaration node to dump to the output
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
 *      output = zenit_variable_node_dump(variable, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_variable_node_dump(ZenitVariableNode *variable, char *output);

/*
 * Function: zenit_variable_node_free
 *  Frees the memory used by the variable declaration node
 *
 * Parameters:
 *  <ZenitVariableNode> *variable: Variable declaration node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_variable_node_free(ZenitVariableNode *var_node);

#endif /* ZENIT_AST_VARIABLE_H */
