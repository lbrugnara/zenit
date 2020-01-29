#include <stdio.h>
#include "cast.h"

struct ZirCastInstruction* zir_instruction_cast_new(struct ZirOperand *destination, struct ZirOperand *source)
{
    struct ZirCastInstruction *instruction = fl_malloc(sizeof(struct ZirCastInstruction));
    instruction->base.type = ZIR_INSTR_CAST;
    instruction->base.destination = destination;
    instruction->source = source;

    instruction->base.destination->owner = instruction;

    if (instruction->source && instruction->source->owner == NULL)
        instruction->source->owner = instruction;
    
    return instruction;
}

void zir_instruction_cast_free(struct ZirCastInstruction *instruction)
{
    zir_operand_free(instruction->base.destination);

    if (instruction->source && instruction->source->owner == instruction)
        zir_operand_free(instruction->source);

    fl_free(instruction);
}

char* zir_instruction_cast_dump(struct ZirCastInstruction *cast, char *output)
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
