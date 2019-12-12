#ifndef ZENIT_AST_TYPE_H
#define ZENIT_AST_TYPE_H

#include "../node.h"

struct ZenitTypeNode {
    struct ZenitNode base;
    enum ZenitType type;
};

#include "array.h"
#include "uint.h"
#include "reference.h"
#include "struct.h"

#endif /* ZENIT_AST_TYPE_H */
