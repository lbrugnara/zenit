#include <stdio.h>
#include <fllib/Cstring.h>
#include "if-false.h"

ZirIfFalseInstruction* zir_instruction_if_false_new(ZirOperand *destination, ZirOperand *source)
{
    ZirIfFalseInstruction *instruction = fl_malloc(sizeof(ZirIfFalseInstruction));
    instruction->base.type = ZIR_INSTR_IF_FALSE;
    instruction->base.destination = destination;
    instruction->source = source;

    return instruction;
}

void zir_instruction_if_false_free(ZirIfFalseInstruction *instruction)
{
    fl_free(instruction);
}

char* zir_instruction_if_false_dump(ZirIfFalseInstruction *if_false, char *output)
{

    fl_cstring_append(&output, "if_false ");
    output = zir_operand_dump(if_false->source, output);
    fl_cstring_append(&output, " jump ");
    output = zir_operand_dump(if_false->base.destination, output);
    fl_cstring_append(&output, "\n");

    return output;
}
