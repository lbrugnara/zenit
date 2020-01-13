#include "scope.h"

struct ZenitScope* zenit_scope_new(const char *id, enum ZenitScopeType type, struct ZenitScope *parent)
{
    struct ZenitScope *scope = fl_malloc(sizeof(struct ZenitScope));
    scope->parent = parent;
    scope->children = fl_array_new(sizeof(struct ZenitScope*), 0);
    scope->symtable = zenit_symtable_new();
    scope->temp_counter = 0;
    scope->id = fl_cstring_dup(id);
    scope->type = type;

    return scope;
}

void zenit_scope_free(struct ZenitScope *scope)
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

char* zenit_scope_dump(struct ZenitScope *scope, char *output)
{
    fl_cstring_append(&output, "(scope ");

    if (scope->type == ZENIT_SCOPE_GLOBAL)
        fl_cstring_append(&output, "global ");
    else if (scope->type == ZENIT_SCOPE_FUNCTION)
        fl_cstring_vappend(&output, "function %s ", scope->id);
    else if (scope->type == ZENIT_SCOPE_STRUCT)
        fl_cstring_vappend(&output, "struct %s ", scope->id);
    else
        fl_cstring_vappend(&output, "unknown %s ", scope->id);

    output = zenit_symtable_dump(&scope->symtable, output);

    size_t length = fl_array_length(scope->children);
    if (length > 0)
    {
        fl_cstring_append(&output, " ");
        for (size_t i=0; i < length; i++)
        {
            output = zenit_scope_dump(scope->children[i], output);
            if (i != length - 1)
                fl_cstring_append(&output, " ");
        }
    }

    fl_cstring_append(&output, ")");

    return output;
}
