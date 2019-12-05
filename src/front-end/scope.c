#include "scope.h"

struct ZenitScope* zenit_scope_new(const char *id, enum ZenitSymtableType type, struct ZenitScope *parent)
{
    struct ZenitScope *scope = fl_malloc(sizeof(struct ZenitScope));
    scope->parent = parent;
    scope->children = fl_array_new(sizeof(struct ZenitScope*), 0);
    scope->symtable = zenit_symtable_new(type, id);

    return scope;
}

void zenit_scope_free(struct ZenitScope *scope)
{
    if (!scope)
        return;

    if (scope->children)
    {
        for (size_t i=0; i < fl_array_length(scope->children); i++)
            zenit_scope_free(scope->children[i]);

        fl_array_free(scope->children);
    }

    zenit_symtable_free(&scope->symtable);

    fl_free(scope);
}
