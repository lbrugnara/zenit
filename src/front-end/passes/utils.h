#ifndef ZENIT_UTILS_H
#define ZENIT_UTILS_H

#include "../program.h"
#include "../symbol.h"
#include "../ast/ast.h"
#include "../types/system.h"

static inline struct ZenitSymbol* zenit_utils_new_tmp_symbol(struct ZenitProgram *program, struct ZenitNode *node, struct ZenitTypeInfo *typeinfo)
{
    char name[1024] = { 0 };
    snprintf(name, 1024, "%%ro_%s_%u%u", zenit_node_print_type(node), node->location.line, node->location.col);

    struct ZenitSymbol *zenit_symbol = zenit_symbol_new(name, typeinfo);

    zenit_program_add_symbol(program, zenit_symbol);

    return zenit_symbol;
}

static inline struct ZenitSymbol* zenit_utils_get_tmp_symbol(struct ZenitProgram *program, struct ZenitNode *node)
{
    char name[1024] = { 0 };
    snprintf(name, 1024, "%%ro_%s_%u%u", zenit_node_print_type(node), node->location.line, node->location.col);

    return zenit_program_get_symbol(program, name);
}

#endif /* ZENIT_UTILS_H */
