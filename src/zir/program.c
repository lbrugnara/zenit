#include <fllib/Cstring.h>
#include "program.h"
#include "symbol.h"
#include "instructions/operands/pool.h"

ZirProgram* zir_program_new()
{
    ZirProgram *program = fl_malloc(sizeof(ZirProgram));
    program->global = zir_block_new("global", ZIR_BLOCK_GLOBAL, NULL);
    program->current = program->global;
    program->operands = zir_operand_pool_new();

    return program;
}

void zir_program_free(ZirProgram *program)
{
    if (!program)
        return;

    zir_operand_pool_free(program->operands);
        
    zir_block_free(program->global);

    fl_free(program);
}


bool zir_program_enter_block(ZirProgram *program, ZirBlock *block)
{
    if (block->parent != program->current)
        return false;

    program->current = block;
    return true;
}


void zir_program_push_block(ZirProgram *program, ZirBlockType type, const char *name)
{
    // First we search for an existent block with the provided type and name    
    ZirBlock *block = zir_program_get_block(program, type, name);

    if (block == NULL)
    {
        // Add a new block to the program
        block = zir_block_new(name, type, program->current);
        program->current->children = fl_array_append(program->current->children, &block);
    }    

    program->current = block;
}

void zir_program_pop_block(ZirProgram *program)
{
    program->current = program->current->parent;
}

bool zir_program_has_block(ZirProgram *program, ZirBlockType type, const char *name)
{
    ZirBlock *tmp_block = program->current;
    do
    {
        // NOTE: For simplicity we use an array, we could replace this with a hashtable later
        for (size_t i=0; i < fl_array_length(program->current->children); i++)
        {
            ZirBlock *block = program->current->children[i];
            if (flm_cstring_equals(block->id, name) && block->type == type)
                return true;
        }
        if (type == ZIR_BLOCK_STRUCT)
            tmp_block = tmp_block->parent;
        else break;

    } while (tmp_block);

    return false;
}

ZirBlock* zir_program_get_block(ZirProgram *program, ZirBlockType type, const char *name)
{
    ZirBlock *tmp_block = program->current;
    do
    {
        // NOTE: For simplicity we use an array, we could replace this with a hashtable later
        for (size_t i=0; i < fl_array_length(program->current->children); i++)
        {
            ZirBlock *block = program->current->children[i];
            if (flm_cstring_equals(block->id, name) && block->type == type)
                return block;
        }
        if (type == ZIR_BLOCK_STRUCT)
            tmp_block = tmp_block->parent;
        else break;

    } while (tmp_block);

    return NULL;
}

ZirSymbol* zir_program_add_symbol(ZirProgram *program, ZirSymbol *symbol)
{
    return zir_symtable_add(&program->current->symtable, symbol);
}

ZirInstruction* zir_program_emit(ZirProgram *program, ZirInstruction *instruction)
{
    program->current->instructions = fl_array_append(program->current->instructions, &instruction);
    return instruction;
}

char* zir_program_dump(ZirProgram *program)
{
    return zir_block_dump(program->global, fl_cstring_new(0));
}
