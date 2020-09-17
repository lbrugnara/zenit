#include <fllib/Mem.h>
#include <fllib/Cstring.h>
#include <fllib/containers/List.h>
#include "if-false.h"

ZnesIfFalseInstruction* znes_if_false_instruction_new(ZnesUintOperand *offset, ZnesOperand *source)
{
    ZnesIfFalseInstruction *instruction = fl_malloc(sizeof(ZnesIfFalseInstruction));

    instruction->base.kind = ZNES_INSTRUCTION_IF_FALSE;
    instruction->offset = offset;
    instruction->source = source;

    return instruction;
}

void znes_if_false_instruction_free(ZnesIfFalseInstruction *instruction)
{
    // No need to free the source or the offset
    fl_free(instruction);
}

char* znes_if_false_instruction_dump(ZnesIfFalseInstruction *instruction, char *output)
{
    fl_cstring_append(&output, "if_false ");
    output = znes_operand_dump(instruction->source, output);
    fl_cstring_append(&output, " jump ");
    output = znes_uint_operand_dump(instruction->offset, output);

    return output;
}
