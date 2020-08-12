#include <fllib/Mem.h>
#include <fllib/containers/List.h>
#include "instr.h"
#include "alloc.h"

void znes_instruction_free(ZnesInstruction *instruction)
{
    switch (instruction->kind)
    {
        case ZNES_INSTRUCTION_ALLOC:
            znes_alloc_instruction_free((ZnesAllocInstruction*) instruction);
            break;

        case ZNES_INSTRUCTION_UNK:
            // TODO: Error handling here?
            break;
    }
}

char* znes_instruction_dump(ZnesInstruction *instruction, char *output)
{
    switch (instruction->kind)
    {
        case ZNES_INSTRUCTION_ALLOC:
            output = znes_alloc_instruction_dump((ZnesAllocInstruction*) instruction, output);
            break;

        case ZNES_INSTRUCTION_UNK:
            // TODO: Error handling here?
            break;
    }

    return output;
}
