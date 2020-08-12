#ifndef RP2A03_EMIT_ALLOC_REF_H
#define RP2A03_EMIT_ALLOC_REF_H

#include "program.h"
#include "../ir/program.h"
#include "../ir/instructions/alloc.h"

static inline bool emit_alloc_from_reference(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAllocInstruction *instruction)
{
    ZnesReferenceOperand *reference_operand = (ZnesReferenceOperand*) instruction->source;
    ZnesAlloc *ref_variable = reference_operand->operand->variable;

    // TODO: Should we loop through the temp object?
    if (ref_variable->type == ZNES_ALLOC_TEMP)
        ref_variable = ((ZnesVariableOperand*) ((ZnesTempAlloc*) ref_variable)->source)->variable;

    if (ref_variable == NULL)
        return false;

    if (instruction->destination->segment == ZNES_SEGMENT_ZP)
    {
        rp2a03_program_emit_imm(program, segment, NES_OP_LDA, (uint8_t)(ref_variable->address & 0xFF));
        rp2a03_program_emit_imm(program, segment, NES_OP_LDX, (uint8_t)((ref_variable->address >> 8) & 0xFF));
        rp2a03_program_emit_zpg(program, segment, NES_OP_STA, (uint8_t)(instruction->destination->address));
        rp2a03_program_emit_zpg(program, segment, NES_OP_STX, (uint8_t)(instruction->destination->address + 1));
    }
    else if (instruction->destination->segment == ZNES_SEGMENT_TEXT)
    {
        rp2a03_program_emit_imm(program, segment, NES_OP_LDA, (uint8_t)(ref_variable->address & 0xFF));
        rp2a03_program_emit_imm(program, segment, NES_OP_LDX, (uint8_t)((ref_variable->address >> 8) & 0xFF));
        rp2a03_program_emit_abs(program, segment, NES_OP_STA, instruction->destination->address);
        rp2a03_program_emit_abs(program, segment, NES_OP_STX, instruction->destination->address + 1);
    }
    else if (instruction->destination->segment == ZNES_SEGMENT_DATA)
    {
        if (is_startup)
        {
            uint8_t *data_seg_slot = program->data->bytes + (instruction->destination->address - program->data->base_address);
            *data_seg_slot       = ref_variable->address & 0xFF;
            *(data_seg_slot+1)   = (ref_variable->address >> 8) & 0xFF;
        }
        else
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDA, (uint8_t)(ref_variable->address & 0xFF));
            rp2a03_program_emit_imm(program, segment, NES_OP_LDX, (uint8_t)((ref_variable->address >> 8) & 0xFF));
            rp2a03_program_emit_abs(program, segment, NES_OP_STA, instruction->destination->address);
            rp2a03_program_emit_abs(program, segment, NES_OP_STX, instruction->destination->address + 1);
        }
    }

    return true;
}

#endif /* RP2A03_EMIT_ALLOC_REF_H */
