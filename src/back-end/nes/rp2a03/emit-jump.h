#ifndef RP2A03_EMIT_JUMP_H
#define RP2A03_EMIT_JUMP_H

#include "program.h"
#include "../ir/program.h"
#include "../ir/instructions/jump.h"

static inline bool rp2a03_emit_jump_instruction(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesJumpInstruction *instruction)
{
    // If bool_value is 0, the Z flag is equals to 1 which means the expression is "false"
    // and it means we should jump
    rp2a03_program_emit_abs(program, segment, NES_OP_JMP, 0);

    // Create a pending jump for the BEQ instruction
    Rp2a03PendingJump pending_jump = {
        // Base PC is the PC used in the calculation
        .base_jump_pc = segment->pc,
        // The index to the value to be backpatched in the BEQ instruction
        .byte_index = segment->pc - 2,
        // The offset in ZNES IR instructions (this is NOT the offset in R2A03 instructions)
        .ir_offset = instruction->offset->size == ZNES_UINT_8 ? instruction->offset->value.uint8 : instruction->offset->value.uint16,
        .absolute = true
    };

    // Add the pending jump
    rp2a03_text_segment_add_pending_jump(segment, &pending_jump);

    return true;
}

#endif /* RP2A03_EMIT_JUMP_H */
