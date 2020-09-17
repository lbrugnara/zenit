#ifndef RP2A03_EMIT_IF_FALSE_H
#define RP2A03_EMIT_IF_FALSE_H

#include "program.h"
#include "../ir/program.h"
#include "../ir/instructions/if-false.h"

static inline bool rp2a03_emit_if_false_instruction(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesIfFalseInstruction *instruction)
{
    if (instruction->source->type == ZNES_OPERAND_BOOL)
    {
        // Get the source value from the bool operand
        uint8_t bool_value = ((ZnesBoolOperand*) instruction->source)->value ? 0x1 : 0x0;

        // Load the immediate value
        rp2a03_program_emit_imm(program, segment, NES_OP_LDA, bool_value);

        // If bool_value is 0, the Z flag is equals to 1 which means the expression is "false"
        // and it means we should jump
        rp2a03_program_emit_rel(program, segment, NES_OP_BEQ, 0);

        // Create a pending jump for the BEQ instruction
        Rp2a03PendingJump pending_jump = {
            // Base PC is the PC used in the calculation
            .base_jump_pc = segment->pc,
            // The index to the value to be backpatched in the BEQ instruction
            .byte_index = segment->pc - 1,
            // The offset in ZNES IR instructions (this is NOT the offset in R2A03 instructions)
            .ir_offset = instruction->offset->size == ZNES_UINT_8 ? instruction->offset->value.uint8 : instruction->offset->value.uint16,
            .absolute = false
        };

        // Add the pending jump
        rp2a03_text_segment_add_pending_jump(segment, &pending_jump);
    }
    else if (instruction->source->type == ZNES_OPERAND_VARIABLE)
    {
        ZnesVariableOperand *variable_operand = (ZnesVariableOperand*) instruction->source;

        if (variable_operand->variable->type != ZNES_ALLOC_TYPE_BOOL)
        {
            // TODO: Handle error, we don't support non-bool expressions on ifs
            return false;
        }

        ZnesAlloc *source_allocation = variable_operand->variable;

        if (variable_operand->variable->segment == ZNES_SEGMENT_DATA || variable_operand->variable->segment == ZNES_SEGMENT_TEXT)
        {
            rp2a03_program_emit_abs(program, segment, NES_OP_LDA, (source_allocation->address));
            for (size_t i=1; i < source_allocation->size; i++)
                rp2a03_program_emit_abs(program, segment, NES_OP_ORA, (source_allocation->address + i));
        }
        else if (variable_operand->variable->segment == ZNES_SEGMENT_ZP)
        {
            rp2a03_program_emit_zpg(program, segment, NES_OP_LDA, (uint8_t) (source_allocation->address));
            for (size_t i=1; i < source_allocation->size; i++)
                rp2a03_program_emit_zpg(program, segment, NES_OP_ORA, (uint8_t) (source_allocation->address + i));
        }
        else if (variable_operand->variable->segment == ZNES_SEGMENT_TEMP)
        {

        }

        // If bool_value is 0, the Z flag is equals to 1 which means the expression is "false"
        // and it means we should jump
        rp2a03_program_emit_rel(program, segment, NES_OP_BEQ, 0);

        // Create a pending jump for the BEQ instruction
        Rp2a03PendingJump pending_jump = {
            // Base PC is the PC used in the calculation
            .base_jump_pc = segment->pc,
            // The index to the value to be backpatched in the BEQ instruction
            .byte_index = segment->pc - 1,
            // The offset in ZNES IR instructions (this is NOT the offset in R2A03 instructions)
            .ir_offset = instruction->offset->size == ZNES_UINT_8 ? instruction->offset->value.uint8 : instruction->offset->value.uint16,
            .absolute = false
        };

        // Add the pending jump
        rp2a03_text_segment_add_pending_jump(segment, &pending_jump);
    }

    return true;
}

#endif /* RP2A03_EMIT_IF_FALSE_H */
