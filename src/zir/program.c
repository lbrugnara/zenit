#include "program.h"
#include "symbol.h"

struct ZirProgram* zir_program_new()
{
    struct ZirProgram *program = fl_malloc(sizeof(struct ZirProgram));
    program->global = zir_block_new("global", ZIR_SYMTABLE_GLOBAL, NULL);
    program->current = program->global;
    program->type_pool = zir_type_pool_new();

    return program;
}

void zir_program_free(struct ZirProgram *program)
{
    if (!program)
        return;
        
    zir_block_free(program->global);

    zir_type_pool_free(&program->type_pool);

    fl_free(program);
}

struct ZirSymbol* zir_program_add_symbol(struct ZirProgram *program, struct ZirSymbol *symbol)
{
    return zir_symtable_add(&program->current->symtable, symbol);
}

struct ZirInstruction* zir_program_emit(struct ZirProgram *program, struct ZirInstruction *instruction)
{
    program->current->instructions = fl_array_append(program->current->instructions, &instruction);
    return instruction;
}

char* zir_program_dump(struct ZirProgram *program, char *output)
{
    return zir_block_dump(program->global, output);
}
