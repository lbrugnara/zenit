#ifndef ZENIT_AST_TYPE_H
#define ZENIT_AST_TYPE_H

#include "node.h"

struct ZenitTypeNode {
    struct ZenitNode base;
    enum ZenitType type;
};

#include "type_array.h"
#include "type_uint.h"
#include "type_reference.h"
#include "type_struct.h"

#endif /* ZENIT_AST_TYPE_H */
