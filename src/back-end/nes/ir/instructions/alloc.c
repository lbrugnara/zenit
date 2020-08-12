#include <fllib/Mem.h>
#include <fllib/Cstring.h>
#include <fllib/containers/List.h>
#include "alloc.h"

ZnesAllocInstruction* znes_alloc_instruction_new(ZnesAlloc *destination, ZnesOperand *source)
{
    ZnesAllocInstruction *instruction = fl_malloc(sizeof(ZnesAllocInstruction));

    instruction->base.kind = ZNES_INSTRUCTION_ALLOC;
    instruction->destination = destination;
    instruction->source = source;

    return instruction;
}

void znes_alloc_instruction_free(ZnesAllocInstruction *instruction)
{
    // No need to free the source or the destination
    fl_free(instruction);
}

char* znes_alloc_instruction_dump(ZnesAllocInstruction *instruction, char *output)
{
    fl_cstring_vappend(&output, "%s = ", instruction->destination->name);
    output = znes_operand_dump(instruction->source, output);
    return output;
}
