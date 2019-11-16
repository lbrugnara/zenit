#include "block.h"

struct ZenitIrBlock* zenit_ir_block_new(const char *id, enum ZenitIrSymbolTableType type, struct ZenitIrBlock *parent)
{
    struct ZenitIrBlock *block = fl_malloc(sizeof(struct ZenitIrBlock));
    block->parent = parent;
    block->instructions = fl_array_new(sizeof(struct ZenitIrInstruction*), 0);
    block->children = fl_array_new(sizeof(struct ZenitIrBlock*), 0);
    block->symtable = zenit_ir_symtable_new(type, id);

    return block;
}

void zenit_ir_block_free(struct ZenitIrBlock *block)
{
    if (!block)
        return;

    if (block->children)
    {
        for (size_t i=0; i < fl_array_length(block->children); i++)
            zenit_ir_block_free(block->children[i]);

        fl_array_free(block->children);
    }

    if (block->instructions)
    {
        for (size_t i=0; i < fl_array_length(block->instructions); i++)
            zenit_ir_instruction_free(block->instructions[i]);

        fl_array_free(block->instructions);
    }

    zenit_ir_symtable_free(&block->symtable);

    fl_free(block);
}

char* zenit_ir_block_dump(struct ZenitIrBlock *block, char *output)
{
    if (block->instructions)
        for (size_t i=0; i < fl_array_length(block->instructions); i++)
            output = zenit_ir_instruction_dump(block->instructions[i], output);

    if (block->children)
        for (size_t i=0; i < fl_array_length(block->children); i++)
            output = zenit_ir_block_dump(block->children[i], output);

    return output;
}
