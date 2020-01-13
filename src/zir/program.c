#include "program.h"
#include "symbol.h"

struct ZirProgram* zir_program_new()
{
    struct ZirProgram *program = fl_malloc(sizeof(struct ZirProgram));
    program->global = zir_block_new("global", ZIR_BLOCK_GLOBAL, NULL);
    program->current = program->global;

    return program;
}

void zir_program_free(struct ZirProgram *program)
{
    if (!program)
        return;
        
    zir_block_free(program->global);

    fl_free(program);
}

void zir_program_push_block(struct ZirProgram *program, enum ZirBlockType type, const char *name)
{
    // First we search for an existent block with the provided type and name    
    struct ZirBlock *block = zir_program_get_block(program, type, name);

    if (block == NULL)
    {
        // Add a new block to the program
        block = zir_block_new(name, type, program->current);
        program->current->children = fl_array_append(program->current->children, &block);
    }    

    program->current = block;
}

void zir_program_pop_block(struct ZirProgram *program)
{
    program->current = program->current->parent;
}

bool zir_program_has_block(struct ZirProgram *program, enum ZirBlockType type, const char *name)
{
    // NOTE: For simplicity we use an array, we could replace this with a hashtable later
    for (size_t i=0; i < fl_array_length(program->current->children); i++)
    {
        struct ZirBlock *block = program->current->children[i];
        if (flm_cstring_equals(block->id, name) && block->type == type)
            return true;
    }

    return false;
}

struct ZirBlock* zir_program_get_block(struct ZirProgram *program, enum ZirBlockType type, const char *name)
{
    // NOTE: For simplicity we use an array, we could replace this with a hashtable later
    for (size_t i=0; i < fl_array_length(program->current->children); i++)
    {
        struct ZirBlock *block = program->current->children[i];
        if (flm_cstring_equals(block->id, name) && block->type == type)
            return block;
    }

    return NULL;
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
