#include "struct.h"

#include "emit.h"

#include "../symbols/struct.h"

void znes_emitter_struct_store(ZnesProgram *program, ZirStructOperand *struct_operand, ZnesSymbol *nes_symbol, size_t offset)
{
    if (nes_symbol->symkind != ZNES_SYMBOL_STRUCT)
    {
        // FIXME: Add error handling for this situation
        return;
    }

    ZnesStructSymbol *struct_symbol = (ZnesStructSymbol*) nes_symbol;

    size_t gap = 0;
    for (size_t i=0; i < fl_array_length(struct_operand->members); i++)
    {
        ZnesSymbol *member_symbol = struct_symbol->members[i];
        ZirStructOperandMember *operand_member = struct_operand->members[i];
        ZirStructTypeMember *type_member = zir_struct_type_get_member(struct_operand->type, operand_member->name);

        znes_emitter_store(program, operand_member->operand, member_symbol, 0);
        gap += member_symbol->size;
    }
}
