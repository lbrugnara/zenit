#ifndef ZENIT_AST_TYPE_STRUCT_H
#define ZENIT_AST_TYPE_STRUCT_H

#include "type.h"

struct ZenitStructTypeNode {
    struct ZenitTypeNode base;
    char *name;
    struct ZenitTypeNode **members;
};

struct ZenitStructTypeNode* zenit_node_type_struct_new(struct ZenitSourceLocation location, char *name);
char* zenit_node_type_struct_uid(struct ZenitStructTypeNode *type_node);
void zenit_node_type_struct_free(struct ZenitStructTypeNode *type_node);

#endif /* ZENIT_AST_TYPE_STRUCT_H */
