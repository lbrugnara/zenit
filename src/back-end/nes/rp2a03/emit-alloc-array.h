#ifndef RP2A03_EMIT_ALLOC_ARRAY_H
#define RP2A03_EMIT_ALLOC_ARRAY_H

#include "program.h"
#include "../ir/program.h"
#include "../ir/objects/array.h"
#include "../ir/instructions/alloc.h"

static inline bool rp2a03_emit_alloc_instruction(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAllocInstruction *instruction);

static inline bool emit_alloc_from_array(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAllocInstruction *instruction)
{
    if (instruction->destination->type != ZNES_ALLOC_TYPE_ARRAY)
    {
        // FIXME: Add error handling for this situation
        return false;
    }

    // Get the source array
    ZnesArrayOperand *source_array = (ZnesArrayOperand*) instruction->source;

    // Get the destination variable
    ZnesArrayAlloc *dest_array = (ZnesArrayAlloc*) instruction->destination;
    
    for (size_t i=0; i < fl_array_length(dest_array->elements); i++)
    {
        ZnesAlloc *dest_var = dest_array->elements[i];
        ZnesOperand *source_item = source_array->elements[i];

        rp2a03_emit_alloc_instruction(program, segment, is_startup , &znes_alloc_instruction_new_local(dest_var, source_item));
    }

    return true;
}

#endif /* RP2A03_EMIT_ALLOC_ARRAY_H */
