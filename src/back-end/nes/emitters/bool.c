#include "bool.h"

#include "array.h"
#include "reference.h"
#include "struct.h"
#include "symbol.h"
#include "temp.h"

#include "../symbols/array.h"
#include "../symbols/reference.h"
#include "../symbols/struct.h"
#include "../symbols/temp.h"
#include "../symbols/bool.h"

void znes_emitter_bool_store(ZnesProgram *program, ZirBoolOperand *bool_operand, ZnesSymbol *nes_symbol, size_t offset)
{
    uint16_t target_address = nes_symbol->address + offset;

    ZnesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;

    uint8_t bool_value = bool_operand->value ? 0x1 : 0x0;

    if (nes_symbol->segment == ZNES_SEGMENT_ZP)
    {
        znes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(bool_value & 0xFF));
        znes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address));

        // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
        // bool_value (if it is uint8_t they will be all 0)
        if (nes_symbol->size >= 2 /*bytes*/)
        {
            znes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((bool_value >> 8) & 0xFF));
            znes_program_emit_zpg(target_segment, NES_OP_STX, (uint8_t)(target_address + 1));
        }

        // If destination is greater than bool16, fill the destination with 0s
        if (nes_symbol->size > 2 /*bytes*/)
        {
            znes_program_emit_imm(target_segment, NES_OP_LDX, 0x0);
            size_t to_fill = nes_symbol->size - 2;
            for (size_t i=0; i < to_fill; i++)
                znes_program_emit_zpg(target_segment, NES_OP_STX, (uint8_t)(target_address + i + 2));
        }
    }
    else if (nes_symbol->segment == ZNES_SEGMENT_CODE)
    {
        znes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(bool_value & 0xFF));
        znes_program_emit_abs(target_segment, NES_OP_STA, target_address);

        // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
        // bool_value (if it is uint8_t they will be all 0)
        if (nes_symbol->size >= 2 /*bytes*/)
        {
            znes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((bool_value >> 8) & 0xFF));
            znes_program_emit_abs(target_segment, NES_OP_STX, target_address + 1);
        }

        // If destination is greater than bool16, fill the destination with 0s
        if (nes_symbol->size > 2 /*bytes*/)
        {
            znes_program_emit_imm(target_segment, NES_OP_LDX, 0x0);
            size_t to_fill = nes_symbol->size - 2;
            for (size_t i=0; i < to_fill; i++)
                znes_program_emit_abs(target_segment, NES_OP_STX, target_address + i + 2);
        }
    }
    else if (nes_symbol->segment == ZNES_SEGMENT_DATA)
    {
        if (program->static_context)
        {
            uint8_t *slot = program->data.bytes + (target_address - program->data.base_address);
            *slot = bool_value & 0xFF;
            
            // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
            // bool_value (if it is uint8_t they will be all 0)
            if (nes_symbol->size >= 2 /*bytes*/)
                *(slot+1)   = (bool_value >> 8) & 0xFF;

            // If destination is greater than bool16, fill the destination with 0s
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
            znes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(bool_value & 0xFF));
            znes_program_emit_abs(target_segment, NES_OP_STA, target_address);

            // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
            // bool_value (if it is uint8_t they will be all 0)
            if (nes_symbol->size >= 2 /*bytes*/)
            {
                znes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((bool_value >> 8) & 0xFF));
                znes_program_emit_abs(target_segment, NES_OP_STX, target_address + 1);
            }

            // If destination is greater than bool16, fill the destination with 0s
            if (nes_symbol->size > 2 /*bytes*/)
            {
                znes_program_emit_imm(target_segment, NES_OP_LDX, 0x0);
                size_t to_fill = nes_symbol->size - 2;
                for (size_t i=0; i < to_fill; i++)
                    znes_program_emit_abs(target_segment, NES_OP_STX, target_address + i + 2);
            }
        }
    }
}
