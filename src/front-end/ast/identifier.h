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

struct ZenitIdentifierNode* zenit_node_identifier_new(struct ZenitSourceLocation location, char *name);
char* zenit_node_identifier_uid(struct ZenitIdentifierNode *identifier);
void zenit_node_identifier_free(struct ZenitIdentifierNode *identifier_node);

#endif /* ZENIT_AST_IDENTIFIER_H */
