#include <stdio.h>
#include <fllib/Cstring.h>
#include "jump.h"

ZirJumpInstruction* zir_instruction_jump_new(ZirOperand *destination)
{
    ZirJumpInstruction *instruction = fl_malloc(sizeof(ZirJumpInstruction));
    instruction->base.type = ZIR_INSTR_JUMP;
    instruction->base.destination = destination;

    return instruction;
}

void zir_instruction_jump_free(ZirJumpInstruction *instruction)
{
    fl_free(instruction);
}

char* zir_instruction_jump_dump(ZirJumpInstruction *cast, char *output)
{
    fl_cstring_append(&output, "jump ");
    output = zir_operand_dump(cast->base.destination, output);
    fl_cstring_append(&output, "\n");

    return output;
}
