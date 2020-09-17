#include <fllib/Mem.h>
#include <fllib/containers/List.h>
#include "instr.h"
#include "alloc.h"
#include "if-false.h"
#include "jump.h"

void znes_instruction_free(ZnesInstruction *instruction)
{
    switch (instruction->kind)
    {
        case ZNES_INSTRUCTION_ALLOC:
            znes_alloc_instruction_free((ZnesAllocInstruction*) instruction);
            break;

        case ZNES_INSTRUCTION_IF_FALSE:
            znes_if_false_instruction_free((ZnesIfFalseInstruction*) instruction);
            break;

        case ZNES_INSTRUCTION_JUMP:
            znes_jump_instruction_free((ZnesJumpInstruction*) instruction);
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

        case ZNES_INSTRUCTION_IF_FALSE:
            output = znes_if_false_instruction_dump((ZnesIfFalseInstruction*) instruction, output);
            break;

        case ZNES_INSTRUCTION_JUMP:
            output = znes_jump_instruction_dump((ZnesJumpInstruction*) instruction, output);
            break;

        case ZNES_INSTRUCTION_UNK:
            // TODO: Error handling here?
            break;
    }

    return output;
}
