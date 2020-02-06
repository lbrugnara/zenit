#include "uint.h"

#include "array.h"
#include "reference.h"
#include "struct.h"
#include "symbol.h"
#include "temp.h"

#include "../symbols/array.h"
#include "../symbols/reference.h"
#include "../symbols/struct.h"
#include "../symbols/temp.h"
#include "../symbols/uint.h"

void zenit_nes_emitter_uint_store(struct ZenitNesProgram *program, struct ZirUintOperand *uint_operand, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    // The store uint on a temp symbol is just the assignment of the uint operand to the "source" property of the temp symbol
    if (nes_symbol->symkind == ZENIT_NES_SYMBOL_TEMP)
    {
        ((struct ZenitNesTempSymbol*) nes_symbol)->source = (struct ZirOperand*) uint_operand;
        return;
    }

    uint16_t target_address = nes_symbol->address + offset;

    struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;

    // NOTE: We use a uint16_t because it can hold all the current uint values
    uint16_t uint_value = 0;

    if (uint_operand->type->size == ZIR_UINT_8)
    {
        uint_value = uint_operand->value.uint8;
    }
    else if (uint_operand->type->size == ZIR_UINT_16)
    {
        uint_value = uint_operand->value.uint16;
    }
    else
    {
        // FIXME: Add an error
        return;
    }

    if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP)
    {
        zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(uint_value & 0xFF));
        zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address));

        // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
        // uint_value (if it is uint8_t they will be all 0)
        if (nes_symbol->size >= 2 /*bytes*/)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((uint_value >> 8) & 0xFF));
            zenit_nes_program_emit_zpg(target_segment, NES_OP_STX, (uint8_t)(target_address + 1));
        }

        // If destination is greater than uint16, fill the destination with 0s
        if (nes_symbol->size > 2 /*bytes*/)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, 0x0);
            size_t to_fill = nes_symbol->size - 2;
            for (size_t i=0; i < to_fill; i++)
                zenit_nes_program_emit_zpg(target_segment, NES_OP_STX, (uint8_t)(target_address + i + 2));
        }
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
    {
        zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(uint_value & 0xFF));
        zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address);

        // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
        // uint_value (if it is uint8_t they will be all 0)
        if (nes_symbol->size >= 2 /*bytes*/)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((uint_value >> 8) & 0xFF));
            zenit_nes_program_emit_abs(target_segment, NES_OP_STX, target_address + 1);
        }

        // If destination is greater than uint16, fill the destination with 0s
        if (nes_symbol->size > 2 /*bytes*/)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, 0x0);
            size_t to_fill = nes_symbol->size - 2;
            for (size_t i=0; i < to_fill; i++)
                zenit_nes_program_emit_abs(target_segment, NES_OP_STX, target_address + i + 2);
        }
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
    {
        if (program->static_context)
        {
            uint8_t *slot = program->data.bytes + (target_address - program->data.base_address);
            *slot = uint_value & 0xFF;
            
            // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
            // uint_value (if it is uint8_t they will be all 0)
            if (nes_symbol->size >= 2 /*bytes*/)
                *(slot+1)   = (uint_value >> 8) & 0xFF;

            // If destination is greater than uint16, fill the destination with 0s
            if (nes_symbol->size > 2)
            {
                slot += 2;
                size_t to_fill = nes_symbol->size - 2;
                for (size_t i=0; i < to_fill; i++)
                    *(slot + i) = 0;
            }
        }
        else
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(uint_value & 0xFF));
            zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address);

            // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
            // uint_value (if it is uint8_t they will be all 0)
            if (nes_symbol->size >= 2 /*bytes*/)
            {
                zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((uint_value >> 8) & 0xFF));
                zenit_nes_program_emit_abs(target_segment, NES_OP_STX, target_address + 1);
            }

            // If destination is greater than uint16, fill the destination with 0s
            if (nes_symbol->size > 2 /*bytes*/)
            {
                zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, 0x0);
                size_t to_fill = nes_symbol->size - 2;
                for (size_t i=0; i < to_fill; i++)
                    zenit_nes_program_emit_abs(target_segment, NES_OP_STX, target_address + i + 2);
            }
        }
    }
}
