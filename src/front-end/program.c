#include "program.h"
#include "symbol.h"

struct ZenitProgram* zenit_program_new()
{
    struct ZenitProgram *program = fl_malloc(sizeof(struct ZenitProgram));
    program->global_scope = zenit_scope_new("global", ZENIT_SCOPE_GLOBAL, NULL);
    program->current_scope = program->global_scope;

    return program;
}

void zenit_program_free(struct ZenitProgram *program)
{
    if (!program)
        return;
        
    zenit_scope_free(program->global_scope);

    fl_free(program);
}

void zenit_program_add_scope(struct ZenitProgram *program, enum ZenitScopeType type, const char *name)
{
    struct ZenitScope *scope = zenit_scope_new(name, type, program->current_scope);
    program->current_scope->children = fl_array_append(program->current_scope->children, &scope);
}

void zenit_program_enter_scope(struct ZenitProgram *program, struct ZenitScope *scope)
{
    if (scope->parent != program->current_scope)
        return;

    program->current_scope = scope;
}

void zenit_program_push_scope(struct ZenitProgram *program, enum ZenitScopeType type, const char *name)
{
    // First we search for an existent scope with the provided type and name    
    struct ZenitScope *scope = zenit_program_get_scope(program, type, name);

    if (scope == NULL)
    {
        // Add a new scope to the program
        scope = zenit_scope_new(name, type, program->current_scope);
        program->current_scope->children = fl_array_append(program->current_scope->children, &scope);
    }    

    program->current_scope = scope;
}

void zenit_program_pop_scope(struct ZenitProgram *program)
{
    program->current_scope = program->current_scope->parent;
}

bool zenit_program_has_scope(struct ZenitProgram *program, enum ZenitScopeType type, const char *name)
{
    // NOTE: For simplicity we use an array, we could replace this with a hashtable later
    for (size_t i=0; i < fl_array_length(program->current_scope->children); i++)
    {
        struct ZenitScope *scope = program->current_scope->children[i];
        if (flm_cstring_equals(scope->id, name) && scope->type == type)
            return true;
    }

    return false;
}

struct ZenitScope* zenit_program_get_scope(struct ZenitProgram *program, enum ZenitScopeType type, const char *name)
{
    // NOTE: For simplicity we use an array, we could replace this with a hashtable later
    for (size_t i=0; i < fl_array_length(program->current_scope->children); i++)
    {
        struct ZenitScope *scope = program->current_scope->children[i];
        if (flm_cstring_equals(scope->id, name) && scope->type == type)
            return scope;
    }

    return NULL;
}

struct ZenitSymbol* zenit_program_add_symbol(struct ZenitProgram *program, struct ZenitSymbol *symbol)
{
    // FIXME: Fix this to add symbols to the global scope
    return zenit_symtable_add(&program->current_scope->symtable, symbol);
}

bool zenit_program_has_symbol(struct ZenitProgram *program, const char *symbol_name)
{
    // FIXME: Fix this to lookup symbols in different scopes
    return zenit_symtable_has(&program->current_scope->symtable, symbol_name);
}

struct ZenitSymbol* zenit_program_get_symbol(struct ZenitProgram *program, const char *symbol_name)
{
    // FIXME: Fix this to lookup symbols in different scopes
    return zenit_symtable_get(&program->current_scope->symtable, symbol_name);
}

struct ZenitSymbol* zenit_program_remove_symbol(struct ZenitProgram *program, const char *symbol_name)
{
    // FIXME: Fix this to lookup symbols in different scopes
    return zenit_symtable_remove(&program->current_scope->symtable, symbol_name);
}

char* zenit_program_dump(struct ZenitProgram *program)
{
    char *output = fl_cstring_dup("(program ");
    
    output = zenit_scope_dump(program->global_scope, output);
    
    fl_cstring_append(&output, ")");

    return output;
}
