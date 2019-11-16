#ifndef ZENIT_AST_LITERAL_H
#define ZENIT_AST_LITERAL_H

#include "node.h"

/*
 * Struct: struct ZenitLiteralNode
 *  Represents a literal value in the program's source.
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object 
 *  <anonymous union> value: Based on the type information the union is populated with the C value
 * 
 */
struct ZenitLiteralNode {
    struct ZenitNode base;    
    union {
        uint8_t uint8;
        uint16_t uint16;
    } value;
};

#endif /* ZENIT_AST_LITERAL_H */
