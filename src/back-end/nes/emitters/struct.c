#include "struct.h"

#include "emit.h"

#include "../symbols/struct.h"

void zenit_nes_emitter_struct_store(struct ZenitNesProgram *program, struct ZirStructOperand *struct_operand, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    if (nes_symbol->symkind != ZENIT_NES_SYMBOL_STRUCT)
    {
        // FIXME: Add error handling for this situation
        return;
    }

    struct ZenitNesStructSymbol *struct_symbol = (struct ZenitNesStructSymbol*) nes_symbol;

    size_t gap = 0;
    for (size_t i=0; i < fl_array_length(struct_operand->members); i++)
    {
        struct ZenitNesSymbol *member_symbol = struct_symbol->members[i];
        struct ZirStructOperandMember *operand_member = struct_operand->members[i];
        struct ZirStructTypeMember *type_member = zir_type_struct_get_member(struct_operand->type, operand_member->name);

        zenit_nes_emitter_store(program, operand_member->operand, member_symbol, 0);
        gap += member_symbol->size;
    }
}
