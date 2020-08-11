#ifndef ZENIT_AST_UINT_H
#define ZENIT_AST_UINT_H

#include <stdint.h>
#include "node.h"
#include "../types/uint.h"

/*
 * Union: ZenitUintValue
 *  Holds the actual unsigned integer value of the uint literal 
 * 
 * Members:
 *  <uint8_t> uint8: Holds the uint8_t representation of the uint literal
 *  <uint16_t> uint16: Holds the uint16_t representation of the uint literal
 * 
 */
typedef union ZenitUintValue {
    uint8_t uint8;
    uint16_t uint16;
} ZenitUintValue;

/*
 * Struct: ZenitUintNode
 *  An AST node that represents a uint literal in the source code
 * 
 * Members:
 *  <ZenitNode> base: Basic information of the node object
 *  <ZenitUintTypeSize> size: The size in bits of the uint literal
 *  <ZenitUintValue> value: The actual value of the uint literal
 * 
 */
typedef struct ZenitUintNode {
    ZenitNode base;
    ZenitUintTypeSize size;
    ZenitUintValue value;
} ZenitUintNode;

/*
 * Function: zenit_uint_node_new
 *  Creates a new AST node that represents a uint literal
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the uint literal
 *  <enum ZenitUintTypeSize0> size: The size in bits of the uint literal
 *  <ZenitUintValue> value: The actual value of the uint literal
 *
 * Returns:
 *  ZenitUintNode*: Uint node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_uint_node_free> function
 */
ZenitUintNode* zenit_uint_node_new(ZenitSourceLocation location, ZenitUintTypeSize size, ZenitUintValue value);

/*
 * Function: zenit_uint_node_uid
 *  Returns a UID for the uint node
 *
 * Parameters:
 *  <ZenitUintNode> *uint_node: Uint node
 *
 * Returns:
 *  char*: UID of the uint node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_uint_node_uid(ZenitUintNode *uint_node);

/*
 * Function: zenit_uint_node_dump
 *  Appends a dump of the uint node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitUintNode> *uint_node: Uint node to dump to the output
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
 *      output = zenit_uint_node_dump(uint, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_uint_node_dump(ZenitUintNode *uint_node, char *output);

/*
 * Function: zenit_uint_node_free
 *  Frees the memory used by the uint node
 *
 * Parameters:
 *  <ZenitUintNode> *uint_node: Uint node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_uint_node_free(ZenitUintNode *uint_node);

#endif /* ZENIT_AST_UINT_H */
