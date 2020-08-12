#ifndef RP2A03_EMIT_ALLOC_UINT_H
#define RP2A03_EMIT_ALLOC_UINT_H

#include "program.h"
#include "../ir/program.h"
#include "../ir/instructions/alloc.h"

static inline bool emit_alloc_from_uint(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAllocInstruction *instruction)
{
    // Get the source value from the uint operand
    ZnesUintOperand *source = (ZnesUintOperand*) instruction->source;

    // NOTE: We use a uint16_t because it can hold all the current uint values
    uint16_t source_value = 0;

    if (source->size == ZNES_UINT_8)
    {
        source_value = source->value.uint8;
    }
    else if (source->size == ZNES_UINT_16)
    {
        source_value = source->value.uint16;
    }
    else
    {
        // FIXME: Add an error
        return false;
    }

    // Check for the destination segment to store the source value
    if (instruction->destination->segment == ZNES_SEGMENT_ZP)
    {
        rp2a03_program_emit_imm(program, segment, NES_OP_LDA, (uint8_t)(source_value & 0xFF));
        rp2a03_program_emit_zpg(program, segment, NES_OP_STA, (uint8_t)(instruction->destination->address));

        // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
        // source_value (if it is uint8_t they will be all 0)
        if (instruction->destination->size >= 2 /*bytes*/)
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDX, (uint8_t)((source_value >> 8) & 0xFF));
            rp2a03_program_emit_zpg(program, segment, NES_OP_STX, (uint8_t)(instruction->destination->address + 1));
        }

        // If destination is greater than uint16, fill the destination with 0s
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
        rp2a03_program_emit_imm(program, segment, NES_OP_LDA, (uint8_t)(source_value & 0xFF));
        rp2a03_program_emit_abs(program, segment, NES_OP_STA, instruction->destination->address);

        // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
        // source_value (if it is uint8_t they will be all 0)
        if (instruction->destination->size >= 2 /*bytes*/)
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDX, (uint8_t)((source_value >> 8) & 0xFF));
            rp2a03_program_emit_abs(program, segment, NES_OP_STX, instruction->destination->address + 1);
        }

        // If destination is greater than uint16, fill the destination with 0s
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
            *data_seg_slot = source_value & 0xFF;
            
            // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
            // source_value (if it is uint8_t they will be all 0)
            if (instruction->destination->size >= 2 /*bytes*/)
                *(data_seg_slot+1)   = (source_value >> 8) & 0xFF;

            // If destination is greater than uint16, fill the destination with 0s
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
            rp2a03_program_emit_imm(program, segment, NES_OP_LDA, (uint8_t)(source_value & 0xFF));
            rp2a03_program_emit_abs(program, segment, NES_OP_STA, instruction->destination->address);

            // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
            // source_value (if it is uint8_t they will be all 0)
            if (instruction->destination->size >= 2 /*bytes*/)
            {
                rp2a03_program_emit_imm(program, segment, NES_OP_LDX, (uint8_t)((source_value >> 8) & 0xFF));
                rp2a03_program_emit_abs(program, segment, NES_OP_STX, instruction->destination->address + 1);
            }

            // If destination is greater than uint16, fill the destination with 0s
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

#endif /* RP2A03_EMIT_ALLOC_UINT_H */
