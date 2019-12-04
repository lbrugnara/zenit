#include <stdio.h>
#include "cast.h"

struct ZenitIrCastInstruction* zenit_ir_instruction_cast_new(void)
{
    struct ZenitIrCastInstruction *instruction = fl_malloc(sizeof(struct ZenitIrCastInstruction));
    instruction->base.type = ZENIT_IR_INSTR_CAST;
    
    return instruction;
}

void zenit_ir_instruction_cast_free(struct ZenitIrCastInstruction *instruction)
{
    struct ZenitIrCastInstruction *cast = (struct ZenitIrCastInstruction*)instruction;

    zenit_ir_operand_free(&cast->lvalue);
    zenit_ir_operand_free(&cast->rvalue);

    fl_free(instruction);
}

char* zenit_ir_instruction_cast_dump(struct ZenitIrCastInstruction *cast, char *output)
{
    fl_cstring_vappend(&output, "%s%s : %s = cast(", 
        cast->lvalue.operand.symbol->temporal ? "%" : "@", 
        cast->lvalue.operand.symbol->name,
        zenit_ir_type_to_string(&cast->lvalue.operand.symbol->typeinfo)
    );

    output = zenit_ir_operand_dump(&cast->rvalue, output);
    fl_cstring_vappend(&output, ", %s)", zenit_ir_type_to_string(&cast->lvalue.operand.symbol->typeinfo));
    fl_cstring_append(&output, "\n");

    return output;
}
