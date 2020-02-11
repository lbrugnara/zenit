#ifndef ZENIT_AST_UINT_H
#define ZENIT_AST_UINT_H

#include <stdint.h>
#include "node.h"
#include "../types/uint.h"

/*
 * Union: union ZenitUintValue
 *  Holds the actual unsigned integer value of the uint literal 
 * 
 * Members:
 *  <uint8_t> uint8: Holds the uint8_t representation of the uint literal
 *  <uint16_t> uint16: Holds the uint16_t representation of the uint literal
 * 
 */
union ZenitUintValue {
    uint8_t uint8;
    uint16_t uint16;
};

/*
 * Struct: struct ZenitUintNode
 *  An AST node that represents a uint literal in the source code
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <enum ZenitUintTypeSize> size: The size in bits of the uint literal
 *  <union ZenitUintValue> value: The actual value of the uint literal
 * 
 */
struct ZenitUintNode {
    struct ZenitNode base;
    enum ZenitUintTypeSize size;
    union ZenitUintValue value;
};

/*
 * Function: zenit_node_uint_new
 *  Creates a new AST node that represents a uint literal
 *
 * Parameters:
 *  <struct ZenitSourceLocation> location: Location information about the uint literal
 *  <enum ZenitUintTypeSize0> size: The size in bits of the uint literal
 *  <union ZenitUintValue> value: The actual value of the uint literal
 *
 * Returns:
 *  struct ZenitUintNode*: Uint node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_uint_free> function
 */
struct ZenitUintNode* zenit_node_uint_new(struct ZenitSourceLocation location, enum ZenitUintTypeSize size, union ZenitUintValue value);

/*
 * Function: zenit_node_uint_uid
 *  Returns a UID for the uint node
 *
 * Parameters:
 *  <struct ZenitUintNode> *uint_node: Uint node
 *
 * Returns:
 *  char*: UID of the uint node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_uint_uid(struct ZenitUintNode *uint_node);

/*
 * Function: zenit_node_uint_dump
 *  Appends a dump of the uint node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitUintNode> *uint_node: Uint node to dump to the output
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
 *      output = zenit_node_uint_dump(uint, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_uint_dump(struct ZenitUintNode *uint_node, char *output);

/*
 * Function: zenit_node_uint_free
 *  Frees the memory used by the uint node
 *
 * Parameters:
 *  <struct ZenitUintNode> *uint_node: Uint node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_uint_free(struct ZenitUintNode *uint_node);

#endif /* ZENIT_AST_UINT_H */
