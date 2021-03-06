#include <fllib/Cstring.h>
#include "scope.h"

ZenitScope* zenit_scope_new(const char *id, ZenitScopeType type, ZenitScope *parent)
{
    ZenitScope *scope = fl_malloc(sizeof(ZenitScope));
    scope->parent = parent;
    scope->children = fl_array_new(sizeof(ZenitScope*), 0);
    scope->symtable = zenit_symtable_new();
    scope->temp_counter = 0;
    scope->id = fl_cstring_dup(id);
    scope->type = type;

    return scope;
}

void zenit_scope_free(ZenitScope *scope)
{
    if (!scope)
        return;

    if (scope->id)
        fl_cstring_free(scope->id);

    if (scope->children)
    {
        for (size_t i=0; i < fl_array_length(scope->children); i++)
            zenit_scope_free(scope->children[i]);

        fl_array_free(scope->children);
    }

    zenit_symtable_free(&scope->symtable);

    fl_free(scope);
}

bool zenit_scope_has_symbol(ZenitScope *scope, const char *symbol_name)
{
    return zenit_symtable_has(&scope->symtable, symbol_name);
}

ZenitSymbol* zenit_scope_add_symbol(ZenitScope *scope, ZenitSymbol *symbol)
{
    return zenit_symtable_add(&scope->symtable, symbol);
}

ZenitSymbol* zenit_scope_get_symbol(ZenitScope *scope, const char *symbol_name)
{
    return zenit_symtable_get(&scope->symtable, symbol_name);
}

ZenitSymbol** zenit_scope_get_symbols(ZenitScope *scope, bool include_temporals)
{
    return zenit_symtable_get_all(&scope->symtable, include_temporals);
}

ZenitSymbol* zenit_scope_remove_symbol(ZenitScope *scope, const char *symbol_name)
{
    return zenit_symtable_remove(&scope->symtable, symbol_name);
}

bool zenit_scope_has_symbols(ZenitScope *scope)
{
    return !zenit_symtable_is_empty(&scope->symtable);
}

char* zenit_scope_dump(ZenitScope *scope, char *output, bool verbose)
{
    fl_cstring_append(&output, "(scope ");

    if (scope->type == ZENIT_SCOPE_GLOBAL)
        fl_cstring_append(&output, "global");
    else if (scope->type == ZENIT_SCOPE_FUNCTION)
        fl_cstring_vappend(&output, "function %s", scope->id);
    else if (scope->type == ZENIT_SCOPE_STRUCT)
        fl_cstring_vappend(&output, "struct %s", scope->id);
    else if (scope->type == ZENIT_SCOPE_BLOCK)
        fl_cstring_vappend(&output, "block %s", scope->id);
    else
        fl_cstring_vappend(&output, "unknown %s", scope->id);

    output = zenit_symtable_dump(&scope->symtable, output, verbose);

    size_t length = fl_array_length(scope->children);
    if (length > 0)
    {
        fl_cstring_append(&output, " ");
        for (size_t i=0; i < length; i++)
        {
            output = zenit_scope_dump(scope->children[i], output, verbose);
            if (i != length - 1)
                fl_cstring_append(&output, " ");
        }
    }

    fl_cstring_append(&output, ")");

    return output;
}
