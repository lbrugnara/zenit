#include <stdio.h>
#include "instruction.h"
#include "cast.h"
#include "variable.h"

void zir_instruction_free(struct ZirInstruction *instruction)
{
    if (instruction->type == ZIR_INSTR_VARIABLE)
    {
        zir_instruction_variable_free((struct ZirVariableInstruction*)instruction);
    }
    else if (instruction->type == ZIR_INSTR_CAST)
    {
        zir_instruction_cast_free((struct ZirCastInstruction*)instruction);
    }
}

char* zir_instruction_dump(struct ZirInstruction *instruction, char *output)
{
    if (instruction->type == ZIR_INSTR_VARIABLE)
    {
        output = zir_instruction_variable_dump((struct ZirVariableInstruction*)instruction, output);
    }
    else if (instruction->type == ZIR_INSTR_CAST)
    {
        output = zir_instruction_cast_dump((struct ZirCastInstruction*)instruction, output);
    }

    return output;
}
