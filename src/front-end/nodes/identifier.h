#ifndef ZENIT_AST_IDENTIFIER_H
#define ZENIT_AST_IDENTIFIER_H

#include "node.h"

/*
 * Struct: struct ZenitIdentifierNode
 *  Represents a reference to an identifier
 */
struct ZenitIdentifierNode {
    struct ZenitNode base;
    char *name;
};

#endif /* ZENIT_AST_IDENTIFIER_H */
