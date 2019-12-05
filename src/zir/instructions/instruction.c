#include <stdio.h>
#include "instruction.h"
#include "cast.h"
#include "load.h"
#include "variable.h"

void zir_instruction_free(struct ZirInstruction *instruction)
{
    if (instruction->destination)
        zir_operand_free(instruction->destination);

    switch (instruction->type)
    {
        case ZIR_INSTR_VARIABLE:
            zir_instruction_variable_free((struct ZirVariableInstruction*)instruction);
            break;

        case ZIR_INSTR_CAST:
            zir_instruction_cast_free((struct ZirCastInstruction*)instruction);
            break;

        case ZIR_INSTR_LOAD:
            zir_instruction_load_free((struct ZirLoadInstruction*)instruction);
            break;
    }
}

char* zir_instruction_dump(struct ZirInstruction *instruction, char *output)
{
    switch (instruction->type)
    {
        case ZIR_INSTR_VARIABLE:
            return zir_instruction_variable_dump((struct ZirVariableInstruction*)instruction, output);

        case ZIR_INSTR_CAST:
            return zir_instruction_cast_dump((struct ZirCastInstruction*)instruction, output);

        case ZIR_INSTR_LOAD:
            return zir_instruction_load_dump((struct ZirLoadInstruction*)instruction, output);
    }

    return output;
}
