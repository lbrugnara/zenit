#include <stdio.h>
#include "load.h"

struct ZirLoadInstruction* zir_instruction_load_new(struct ZirOperand *destination)
{
    flm_assert(destination != NULL, "Destination operand must not be NULL");

    struct ZirLoadInstruction *instruction = fl_malloc(sizeof(struct ZirLoadInstruction));
    instruction->base.type = ZIR_INSTR_LOAD;
    instruction->base.destination = destination;
    
    return instruction;
}

void zir_instruction_load_free(struct ZirLoadInstruction *instruction)
{
    fl_free(instruction);
}

char* zir_instruction_load_dump(struct ZirLoadInstruction *load, char *output)
{
    // "Hide" the output of the load instruction
    return output;
}
