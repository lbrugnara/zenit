#ifndef RP2A03_EMIT_ALLOC_STRUCT_H
#define RP2A03_EMIT_ALLOC_STRUCT_H

#include "program.h"
#include "../ir/program.h"
#include "../ir/objects/struct.h"
#include "../ir/instructions/alloc.h"

static inline bool rp2a03_emit_alloc_instruction(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAllocInstruction *instruction);

static inline bool emit_alloc_from_struct(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAllocInstruction *instruction)
{
    if (instruction->destination->type != ZNES_ALLOC_TYPE_STRUCT)
    {
        // FIXME: Add error handling for this situation
        return false;
    }

    ZnesStructOperand *struct_operand = (ZnesStructOperand*) instruction->source;
    ZnesStructAlloc *struct_var = (ZnesStructAlloc*) instruction->destination;

    for (size_t i=0; i < fl_array_length(struct_operand->members); i++)
    {
        ZnesAlloc *member_var = struct_var->members[i];
        ZnesStructOperandMember *struct_member = struct_operand->members[i];
        
        rp2a03_emit_alloc_instruction(program, segment, is_startup, &znes_alloc_instruction_new_local(member_var, struct_member->operand));
    }

    return true;
}

#endif /* RP2A03_EMIT_ALLOC_STRUCT_H */
