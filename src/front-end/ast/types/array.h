#ifndef ZENIT_AST_TYPE_ARRAY_H
#define ZENIT_AST_TYPE_ARRAY_H

#include "type.h"

struct ZenitArrayTypeNode {
    struct ZenitTypeNode base;
    struct ZenitTypeNode *member_type;
    size_t length;
    bool auto_length;
};

struct ZenitArrayTypeNode* zenit_node_type_array_new(struct ZenitSourceLocation location, struct ZenitTypeNode *member_type);
char* zenit_node_type_array_uid(struct ZenitArrayTypeNode *type_node);
char* zenit_node_type_array_dump(struct ZenitArrayTypeNode *type_node, char *output);
char* zenit_node_type_array_to_string(struct ZenitArrayTypeNode *type_node);
void zenit_node_type_array_free(struct ZenitArrayTypeNode *type_node);

#endif /* ZENIT_AST_TYPE_ARRAY_H */
