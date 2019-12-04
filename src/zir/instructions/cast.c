#include <stdio.h>
#include "cast.h"

struct ZirCastInstruction* zir_instruction_cast_new(void)
{
    struct ZirCastInstruction *instruction = fl_malloc(sizeof(struct ZirCastInstruction));
    instruction->base.type = ZIR_INSTR_CAST;
    
    return instruction;
}

void zir_instruction_cast_free(struct ZirCastInstruction *instruction)
{
    struct ZirCastInstruction *cast = (struct ZirCastInstruction*)instruction;

    zir_operand_free(&cast->lvalue);
    zir_operand_free(&cast->rvalue);

    fl_free(instruction);
}

char* zir_instruction_cast_dump(struct ZirCastInstruction *cast, char *output)
{
    fl_cstring_vappend(&output, "%s%s : %s = cast(", 
        cast->lvalue.operand.symbol->temporal ? "%" : "@", 
        cast->lvalue.operand.symbol->name,
        zir_type_to_string(&cast->lvalue.operand.symbol->typeinfo)
    );

    output = zir_operand_dump(&cast->rvalue, output);
    fl_cstring_vappend(&output, ", %s)", zir_type_to_string(&cast->lvalue.operand.symbol->typeinfo));
    fl_cstring_append(&output, "\n");

    return output;
}
