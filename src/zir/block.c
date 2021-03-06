#include <fllib/Cstring.h>
#include "block.h"

ZirBlock* zir_block_new(const char *id, ZirBlockType type, ZirBlock *parent)
{
    ZirBlock *block = fl_malloc(sizeof(ZirBlock));
    block->parent = parent;
    block->instructions = fl_array_new(sizeof(ZirInstr*), 0);
    block->children = fl_array_new(sizeof(ZirBlock*), 0);
    block->symtable = zir_symtable_new();
    block->temp_counter = 0;
    block->id = fl_cstring_dup(id);
    block->type = type;

    return block;
}

void zir_block_free(ZirBlock *block)
{
    if (!block)
        return;

    if (block->id)
        fl_cstring_free(block->id);

    if (block->children)
    {
        for (size_t i=0; i < fl_array_length(block->children); i++)
            zir_block_free(block->children[i]);

        fl_array_free(block->children);
    }

    if (block->instructions)
    {
        for (size_t i=0; i < fl_array_length(block->instructions); i++)
            zir_instruction_free(block->instructions[i]);

        fl_array_free(block->instructions);
    }

    zir_symtable_free(&block->symtable);

    fl_free(block);
}

char* zir_block_dump(ZirBlock *block, char *output)
{
    if (block->children)
        for (size_t i=0; i < fl_array_length(block->children); i++)
            output = zir_block_dump(block->children[i], output);

    if (block->type == ZIR_BLOCK_STRUCT)
    {
        fl_cstring_vappend(&output, "struct %s { ", block->id);

        output = zir_symtable_dump(&block->symtable, output);

        fl_cstring_append(&output, " }\n");
    }

    if (block->instructions)
        for (size_t i=0; i < fl_array_length(block->instructions); i++)
            output = zir_instruction_dump(block->instructions[i], output);

    return output;
}

size_t zir_block_get_ip(ZirBlock *block)
{
    return fl_array_length(block->instructions);
}

bool zir_block_has_symbol(ZirBlock *block, const char *name)
{
    // NOTE: Should we search in the parent here?
    return zir_symtable_has(&block->symtable, name);
}
