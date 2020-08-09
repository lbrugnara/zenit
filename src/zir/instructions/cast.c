#include <stdio.h>
#include <fllib/Cstring.h>
#include "cast.h"

ZirCastInstruction* zir_instruction_cast_new(ZirOperand *destination, ZirOperand *source)
{
    ZirCastInstruction *instruction = fl_malloc(sizeof(ZirCastInstruction));
    instruction->base.type = ZIR_INSTR_CAST;
    instruction->base.destination = destination;
    instruction->source = source;

    return instruction;
}

void zir_instruction_cast_free(ZirCastInstruction *instruction)
{
    fl_free(instruction);
}

char* zir_instruction_cast_dump(ZirCastInstruction *cast, char *output)
{
    output = zir_operand_dump(cast->base.destination, output);
    fl_cstring_append(&output, " : ");
    output = zir_operand_type_dump(cast->base.destination, output);
    fl_cstring_append(&output, " = cast(");
    output = zir_operand_dump(cast->source, output);
    fl_cstring_append(&output, ", ");
    output = zir_operand_type_dump(cast->base.destination, output);
    fl_cstring_append(&output, ")");
    fl_cstring_append(&output, "\n");

    return output;
}
