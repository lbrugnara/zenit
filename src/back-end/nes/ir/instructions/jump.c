#include <fllib/Mem.h>
#include <fllib/Cstring.h>
#include <fllib/containers/List.h>
#include "jump.h"

ZnesJumpInstruction* znes_jump_instruction_new(ZnesUintOperand *offset)
{
    ZnesJumpInstruction *instruction = fl_malloc(sizeof(ZnesJumpInstruction));

    instruction->base.kind = ZNES_INSTRUCTION_JUMP;
    instruction->offset = offset;

    return instruction;
}

void znes_jump_instruction_free(ZnesJumpInstruction *instruction)
{
    // No need to free the source or the offset
    fl_free(instruction);
}

char* znes_jump_instruction_dump(ZnesJumpInstruction *instruction, char *output)
{
    fl_cstring_append(&output, "jump ");
    output = znes_uint_operand_dump(instruction->offset, output);

    return output;
}
