#ifndef RP2A03_EMIT_ALLOC_BOOL_H
#define RP2A03_EMIT_ALLOC_BOOL_H

#include "program.h"
#include "../ir/operands/bool.h"
#include "../ir/program.h"
#include "../ir/instructions/alloc.h"

static inline bool emit_alloc_from_bool(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAllocInstruction *instruction)
{
    // Get the source value from the bool operand
    uint8_t bool_value = ((ZnesBoolOperand*) instruction->source)->value ? 0x1 : 0x0;

    if (instruction->destination->segment == ZNES_SEGMENT_ZP)
    {
        rp2a03_program_emit_imm(program, segment, NES_OP_LDA, (uint8_t)(bool_value & 0xFF));
        rp2a03_program_emit_zpg(program, segment, NES_OP_STA, (uint8_t)(instruction->destination->address));

        // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
        // bool_value (if it is uint8_t they will be all 0)
        if (instruction->destination->size >= 2 /*bytes*/)
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDX, (uint8_t)((bool_value >> 8) & 0xFF));
            rp2a03_program_emit_zpg(program, segment, NES_OP_STX, (uint8_t)(instruction->destination->address + 1));
        }

        // If destination is greater than bool16, fill the destination with 0s
        if (instruction->destination->size > 2 /*bytes*/)
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDX, 0x0);
            size_t to_fill = instruction->destination->size - 2;
            for (size_t i=0; i < to_fill; i++)
                rp2a03_program_emit_zpg(program, segment, NES_OP_STX, (uint8_t)(instruction->destination->address + i + 2));
        }
    }
    else if (instruction->destination->segment == ZNES_SEGMENT_TEXT)
    {
        rp2a03_program_emit_imm(program, segment, NES_OP_LDA, (uint8_t)(bool_value & 0xFF));
        rp2a03_program_emit_abs(program, segment, NES_OP_STA, instruction->destination->address);

        // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
        // bool_value (if it is uint8_t they will be all 0)
        if (instruction->destination->size >= 2 /*bytes*/)
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDX, (uint8_t)((bool_value >> 8) & 0xFF));
            rp2a03_program_emit_abs(program, segment, NES_OP_STX, instruction->destination->address + 1);
        }

        // If destination is greater than bool16, fill the destination with 0s
        if (instruction->destination->size > 2 /*bytes*/)
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDX, 0x0);
            size_t to_fill = instruction->destination->size - 2;
            for (size_t i=0; i < to_fill; i++)
                rp2a03_program_emit_abs(program, segment, NES_OP_STX, instruction->destination->address + i + 2);
        }
    }
    else if (instruction->destination->segment == ZNES_SEGMENT_DATA)
    {
        if (is_startup)
        {
            uint8_t *data_seg_slot = program->data->bytes + (instruction->destination->address - program->data->base_address);
            *data_seg_slot = bool_value & 0xFF;
            
            // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
            // bool_value (if it is uint8_t they will be all 0)
            if (instruction->destination->size >= 2 /*bytes*/)
                *(data_seg_slot+1)   = (bool_value >> 8) & 0xFF;

            // If destination is greater than bool16, fill the destination with 0s
            if (instruction->destination->size > 2)
            {
                data_seg_slot += 2;
                size_t to_fill = instruction->destination->size - 2;
                for (size_t i=0; i < to_fill; i++)
                    *(data_seg_slot + i) = 0;
            }
        }
        else
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDA, (uint8_t)(bool_value & 0xFF));
            rp2a03_program_emit_abs(program, segment, NES_OP_STA, instruction->destination->address);

            // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
            // bool_value (if it is uint8_t they will be all 0)
            if (instruction->destination->size >= 2 /*bytes*/)
            {
                rp2a03_program_emit_imm(program, segment, NES_OP_LDX, (uint8_t)((bool_value >> 8) & 0xFF));
                rp2a03_program_emit_abs(program, segment, NES_OP_STX, instruction->destination->address + 1);
            }

            // If destination is greater than bool16, fill the destination with 0s
            if (instruction->destination->size > 2 /*bytes*/)
            {
                rp2a03_program_emit_imm(program, segment, NES_OP_LDX, 0x0);
                size_t to_fill = instruction->destination->size - 2;
                for (size_t i=0; i < to_fill; i++)
                    rp2a03_program_emit_abs(program, segment, NES_OP_STX, instruction->destination->address + i + 2);
            }
        }
    }

    return true;
}

#endif /* RP2A03_EMIT_ALLOC_BOOL_H */
