#include "program.h"
#include "symbol.h"

struct ZenitProgram* zenit_program_new()
{
    struct ZenitProgram *program = fl_malloc(sizeof(struct ZenitProgram));
    program->global_scope = zenit_scope_new("global", ZENIT_SYMTABLE_GLOBAL, NULL);
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
    return zenit_scope_dump(program->global_scope, fl_cstring_new(0));
}
