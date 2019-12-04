#include <stdio.h>
#include "instruction.h"
#include "cast.h"
#include "variable.h"

void zenit_ir_instruction_free(struct ZenitIrInstruction *instruction)
{
    if (instruction->type == ZENIT_IR_INSTR_VARIABLE)
    {
        zenit_ir_instruction_variable_free((struct ZenitIrVariableInstruction*)instruction);
    }
    else if (instruction->type == ZENIT_IR_INSTR_CAST)
    {
        zenit_ir_instruction_cast_free((struct ZenitIrCastInstruction*)instruction);
    }
}

char* zenit_ir_instruction_dump(struct ZenitIrInstruction *instruction, char *output)
{
    if (instruction->type == ZENIT_IR_INSTR_VARIABLE)
    {
        output = zenit_ir_instruction_variable_dump((struct ZenitIrVariableInstruction*)instruction, output);
    }
    else if (instruction->type == ZENIT_IR_INSTR_CAST)
    {
        output = zenit_ir_instruction_cast_dump((struct ZenitIrCastInstruction*)instruction, output);
    }

    return output;
}
