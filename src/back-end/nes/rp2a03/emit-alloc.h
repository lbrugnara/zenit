#ifndef RP2A03_EMIT_ALLOC_H
#define RP2A03_EMIT_ALLOC_H

#include "program.h"
#include "../ir/program.h"

#include "emit-alloc-array.h"
#include "emit-alloc-bool.h"
#include "emit-alloc-ref.h"
#include "emit-alloc-struct.h"
#include "emit-alloc-uint.h"
#include "emit-alloc-var.h"

static inline bool rp2a03_emit_alloc_instruction(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAllocInstruction *instruction)
{
    // TODO: Temp
    if (instruction->destination->type == ZNES_ALLOC_TEMP)
    {
        ((ZnesTempAlloc*) instruction->destination)->source = instruction->source;
        return true;
    }

    if (instruction->destination->segment == ZNES_SEGMENT_DATA)
    {
        for (size_t i=0; i < instruction->destination->size; i++)
            program->data->slots[instruction->destination->address - program->data->base_address + i] = 1;
    }

    if (instruction->source->type == ZNES_OPERAND_UINT)
        return emit_alloc_from_uint(program, segment, is_startup, instruction);
    
    if (instruction->source->type == ZNES_OPERAND_BOOL)
        return emit_alloc_from_bool(program, segment, is_startup, instruction);
    
    if (instruction->source->type == ZNES_OPERAND_ARRAY)
        return emit_alloc_from_array(program, segment, is_startup, instruction);
    
    if (instruction->source->type == ZNES_OPERAND_STRUCT)
        return emit_alloc_from_struct(program, segment, is_startup, instruction);
    
    if (instruction->source->type == ZNES_OPERAND_REFERENCE)
        return emit_alloc_from_reference(program, segment, is_startup, instruction);
    
    if (instruction->source->type == ZNES_OPERAND_VARIABLE)
        return emit_alloc_from_variable(program, segment, is_startup, instruction);
    
    return false;
}

#endif /* RP2A03_EMIT_ALLOC_H */
