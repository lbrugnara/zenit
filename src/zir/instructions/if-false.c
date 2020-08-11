#include <stdio.h>
#include <fllib/Cstring.h>
#include "if-false.h"

ZirIfFalseInstr* zir_if_false_instr_new(ZirOperand *destination, ZirOperand *source)
{
    ZirIfFalseInstr *instruction = fl_malloc(sizeof(ZirIfFalseInstr));
    instruction->base.type = ZIR_INSTR_IF_FALSE;
    instruction->base.destination = destination;
    instruction->source = source;

    return instruction;
}

void zir_if_false_instr_free(ZirIfFalseInstr *instruction)
{
    fl_free(instruction);
}

char* zir_if_false_instr_dump(ZirIfFalseInstr *if_false, char *output)
{

    fl_cstring_append(&output, "if_false ");
    output = zir_operand_dump(if_false->source, output);
    fl_cstring_append(&output, " jump ");
    output = zir_operand_dump(if_false->base.destination, output);
    fl_cstring_append(&output, "\n");

    return output;
}
