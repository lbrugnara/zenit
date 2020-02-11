#include "reference.h"

#include "array.h"
#include "struct.h"
#include "symbol.h"
#include "temp.h"
#include "uint.h"

#include "../symbols/array.h"
#include "../symbols/reference.h"
#include "../symbols/struct.h"
#include "../symbols/temp.h"
#include "../symbols/uint.h"

void zenit_nes_emitter_reference_store(struct ZenitNesProgram *program, struct ZirReferenceOperand *reference_operand, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    // The store reference on a temp symbol is just the assignment of the reference operand to the "source" property of the temp symbol
    if (nes_symbol->symkind == ZENIT_NES_SYMBOL_TEMP)
    {
        ((struct ZenitNesTempSymbol*) nes_symbol)->source = (struct ZirOperand*) reference_operand;
        return;
    }

    uint16_t target_address = nes_symbol->address + offset;
    struct ZenitNesSymbol *ref_op_symbol = fl_hashtable_get(program->symbols, reference_operand->operand->symbol->name);

    if (ref_op_symbol->symkind == ZENIT_NES_SYMBOL_TEMP)
        ref_op_symbol = zenit_nes_program_get_tmpsym_symbol(program, ref_op_symbol);

    if (ref_op_symbol == NULL)
        return;

    struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;

    if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP)
    {
        zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(ref_op_symbol->address & 0xFF));
        zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((ref_op_symbol->address >> 8) & 0xFF));
        zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address));
        zenit_nes_program_emit_zpg(target_segment, NES_OP_STX, (uint8_t)(target_address + 1));
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
    {
        zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(ref_op_symbol->address & 0xFF));
        zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((ref_op_symbol->address >> 8) & 0xFF));
        zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address);
        zenit_nes_program_emit_abs(target_segment, NES_OP_STX, target_address + 1);
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
    {
        if (program->static_context)
        {
            uint8_t *slot = program->data.bytes + (target_address - program->data.base_address);
            *slot       = ref_op_symbol->address & 0xFF;
            *(slot+1)   = (ref_op_symbol->address >> 8) & 0xFF;
        }
        else
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(ref_op_symbol->address & 0xFF));
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((ref_op_symbol->address >> 8) & 0xFF));
            zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address);
            zenit_nes_program_emit_abs(target_segment, NES_OP_STX, target_address + 1);
        }
    }
}