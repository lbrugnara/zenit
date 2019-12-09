#ifndef ZENIT_UTILS_H
#define ZENIT_UTILS_H

#include "../program.h"
#include "../symbol.h"
#include "../ast/ast.h"
#include "../types/system.h"

static inline struct ZenitSymbol* zenit_utils_new_tmp_symbol(struct ZenitProgram *program, struct ZenitNode *node, struct ZenitTypeInfo *typeinfo)
{
    char *name = zenit_node_uid(node);
    if (zenit_program_has_symbol(program, name))
    {
        fl_cstring_free(name);
        return NULL;
    }

    struct ZenitSymbol *symbol = zenit_symbol_new(name, typeinfo);
    
    struct ZenitSymbol *result = zenit_program_add_symbol(program, symbol);

    if (result == NULL)
        zenit_symbol_free(symbol);

    fl_cstring_free(name);

    return result;
}

static inline struct ZenitSymbol* zenit_utils_get_tmp_symbol(struct ZenitProgram *program, struct ZenitNode *node)
{
    char *name = zenit_node_uid(node);
    struct ZenitSymbol *symbol = zenit_program_get_symbol(program, name);

    fl_cstring_free(name);

    return symbol;
}

#endif /* ZENIT_UTILS_H */
