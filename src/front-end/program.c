#include "program.h"
#include "symbol.h"

struct ZenitProgram* zenit_program_new()
{
    struct ZenitProgram *program = fl_malloc(sizeof(struct ZenitProgram));
    program->global_scope = zenit_scope_new("global", ZENIT_SYMTABLE_GLOBAL, NULL);
    program->current_scope = program->global_scope;
    program->types = zenit_type_pool_new();

    return program;
}

void zenit_program_free(struct ZenitProgram *program)
{
    if (!program)
        return;
        
    zenit_scope_free(program->global_scope);

    zenit_type_pool_free(program->types);

    fl_free(program);
}

struct ZenitSymbol* zenit_program_add_symbol(struct ZenitProgram *program, struct ZenitSymbol *symbol)
{
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

struct ZenitTypeInfo* zenit_program_register_type(struct ZenitProgram *program, struct ZenitTypeInfo *typeinfo)
{
    return zenit_type_pool_register(program->types, typeinfo);
}
