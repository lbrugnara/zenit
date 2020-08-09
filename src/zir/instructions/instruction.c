#include <stdio.h>
#include "instruction.h"
#include "cast.h"
#include "variable.h"

void zir_instruction_free(ZirInstruction *instruction)
{
    switch (instruction->type)
    {
        case ZIR_INSTR_VARIABLE:
            zir_instruction_variable_free((ZirVariableInstruction*)instruction);
            break;

        case ZIR_INSTR_CAST:
            zir_instruction_cast_free((ZirCastInstruction*)instruction);
            break;
    }
}

char* zir_instruction_dump(ZirInstruction *instruction, char *output)
{
    switch (instruction->type)
    {
        case ZIR_INSTR_VARIABLE:
            return zir_instruction_variable_dump((ZirVariableInstruction*)instruction, output);

        case ZIR_INSTR_CAST:
            return zir_instruction_cast_dump((ZirCastInstruction*)instruction, output);
    }

    return output;
}
