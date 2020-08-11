#include <stdio.h>
#include "instruction.h"
#include "cast.h"
#include "if-false.h"
#include "jump.h"
#include "variable.h"

void zir_instruction_free(ZirInstr *instruction)
{
    switch (instruction->type)
    {
        case ZIR_INSTR_VARIABLE:
            zir_variable_instr_free((ZirVariableInstr*) instruction);
            break;

        case ZIR_INSTR_CAST:
            zir_cast_instr_free((ZirCastInstr*) instruction);
            break;

        case ZIR_INSTR_IF_FALSE:
            zir_if_false_instr_free((ZirIfFalseInstr*) instruction);
            break;

        case ZIR_INSTR_JUMP:
            zir_jump_instr_free((ZirJumpInstr*) instruction);
            break;
    }
}

char* zir_instruction_dump(ZirInstr *instruction, char *output)
{
    switch (instruction->type)
    {
        case ZIR_INSTR_VARIABLE:
            return zir_variable_instr_dump((ZirVariableInstr*) instruction, output);

        case ZIR_INSTR_CAST:
            return zir_cast_instr_dump((ZirCastInstr*) instruction, output);

        case ZIR_INSTR_IF_FALSE:
            return zir_if_false_instr_dump((ZirIfFalseInstr*) instruction, output);

        case ZIR_INSTR_JUMP:
            return zir_jump_instr_dump((ZirJumpInstr*) instruction, output);
    }

    return output;
}
