#include "struct.h"

#include "array.h"
#include "reference.h"
#include "symbol.h"
#include "temp.h"
#include "uint.h"

#include "../symbols/array.h"
#include "../symbols/reference.h"
#include "../symbols/struct.h"
#include "../symbols/temp.h"
#include "../symbols/uint.h"

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

        if (operand_member->operand->type == ZIR_OPERAND_UINT)
        {
            zenit_nes_emitter_uint_store(program, (struct ZirUintOperand*) operand_member->operand, member_symbol, 0);
        }
        else if (operand_member->operand->type == ZIR_OPERAND_ARRAY)
        {
            zenit_nes_emitter_array_store(program, (struct ZirArrayOperand*)operand_member->operand, member_symbol, 0);
        }
        else if (operand_member->operand->type == ZIR_OPERAND_STRUCT)
        {
            zenit_nes_emitter_struct_store(program, (struct ZirStructOperand*) operand_member->operand, member_symbol, 0);
        }
        else if (operand_member->operand->type == ZIR_OPERAND_REFERENCE)
        {
            zenit_nes_emitter_reference_store(program, (struct ZirReferenceOperand*) operand_member->operand, member_symbol, 0);
        }
        else if (operand_member->operand->type == ZIR_OPERAND_SYMBOL)
        {
            zenit_nes_emitter_symbol_store(program, (struct ZirSymbolOperand*) operand_member->operand, member_symbol, 0);
        }

        gap += member_symbol->size;
    }
}
