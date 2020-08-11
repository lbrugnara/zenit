#include <stdio.h>
#include <fllib/Cstring.h>
#include "cast.h"

ZirCastInstr* zir_cast_instr_new(ZirOperand *destination, ZirOperand *source)
{
    ZirCastInstr *instruction = fl_malloc(sizeof(ZirCastInstr));
    instruction->base.type = ZIR_INSTR_CAST;
    instruction->base.destination = destination;
    instruction->source = source;

    return instruction;
}

void zir_cast_instr_free(ZirCastInstr *instruction)
{
    fl_free(instruction);
}

char* zir_cast_instr_dump(ZirCastInstr *cast, char *output)
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
