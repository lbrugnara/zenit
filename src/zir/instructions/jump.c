#include <stdio.h>
#include <fllib/Cstring.h>
#include "jump.h"

ZirJumpInstr* zir_jump_instr_new(ZirOperand *destination)
{
    ZirJumpInstr *instruction = fl_malloc(sizeof(ZirJumpInstr));
    instruction->base.type = ZIR_INSTR_JUMP;
    instruction->base.destination = destination;

    return instruction;
}

void zir_jump_instr_free(ZirJumpInstr *instruction)
{
    fl_free(instruction);
}

char* zir_jump_instr_dump(ZirJumpInstr *cast, char *output)
{
    fl_cstring_append(&output, "jump ");
    output = zir_operand_dump(cast->base.destination, output);
    fl_cstring_append(&output, "\n");

    return output;
}
