#include "block.h"

struct ZirBlock* zir_block_new(const char *id, enum ZirSymbolTableType type, struct ZirBlock *parent)
{
    struct ZirBlock *block = fl_malloc(sizeof(struct ZirBlock));
    block->parent = parent;
    block->instructions = fl_array_new(sizeof(struct ZirInstruction*), 0);
    block->children = fl_array_new(sizeof(struct ZirBlock*), 0);
    block->symtable = zir_symtable_new(type, id);
    block->temp_counter = 0;

    return block;
}

void zir_block_free(struct ZirBlock *block)
{
    if (!block)
        return;

    if (block->children)
    {
        //for (size_t i=0; i < fl_array_length(block->children); i++)
        for (size_t i=fl_array_length(block->children); i > 0; i--)
            zir_block_free(block->children[i - 1]);

        fl_array_free(block->children);
    }

    if (block->instructions)
    {
        //for (size_t i=0; i < fl_array_length(block->instructions); i++)
        for (size_t i=fl_array_length(block->instructions); i > 0; i--)
            zir_instruction_free(block->instructions[i - 1]);

        fl_array_free(block->instructions);
    }

    zir_symtable_free(&block->symtable);

    fl_free(block);
}

char* zir_block_dump(struct ZirBlock *block, char *output)
{
    if (block->instructions)
        for (size_t i=0; i < fl_array_length(block->instructions); i++)
            output = zir_instruction_dump(block->instructions[i], output);

    if (block->children)
        for (size_t i=0; i < fl_array_length(block->children); i++)
            output = zir_block_dump(block->children[i], output);

    return output;
}
