#ifndef RP2A03_EMIT_ALLOC_VAR_H
#define RP2A03_EMIT_ALLOC_VAR_H

#include "program.h"
#include "../ir/program.h"
#include "../ir/objects/array.h"
#include "../ir/objects/struct.h"
#include "../ir/instructions/alloc.h"

static inline bool rp2a03_emit_alloc_instruction(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAllocInstruction *instruction);

static bool emit_alloc_from_zp_var_to_zp_var(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAlloc *source, ZnesAlloc *destination)
{
    if (source->type != destination->type)
        return false;

    if (source->type == ZNES_ALLOC_TYPE_ARRAY)
    {
        ZnesArrayAlloc *source_array = (ZnesArrayAlloc*) source;
        ZnesArrayAlloc *dest_array = (ZnesArrayAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_array->elements); i++)
            if (!emit_alloc_from_zp_var_to_zp_var(program, segment, is_startup, source_array->elements[i], dest_array->elements[i]))
                return false;
    }
    else if (source->type == ZNES_ALLOC_TYPE_STRUCT)
    {
        ZnesStructAlloc *source_struct = (ZnesStructAlloc*) source;
        ZnesStructAlloc *dest_struct = (ZnesStructAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_struct->members); i++)
            if (!emit_alloc_from_zp_var_to_zp_var(program, segment, is_startup, source_struct->members[i], dest_struct->members[i]))
                return false;
    }
    else
    {
        // FIXME: fix this suboptimal implementation
        size_t to_copy = source->size > destination->size 
                            ? destination->size 
                            : source->size;

        for (size_t i=0; i < to_copy; i++)
        {
            rp2a03_program_emit_zpg(program, segment, NES_OP_LDA, (uint8_t)(source->address + i));
            rp2a03_program_emit_zpg(program, segment, NES_OP_STA, (uint8_t)(destination->address + i));
        }

        if (destination->size > source->size)
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < destination->size; i++)
                rp2a03_program_emit_zpg(program, segment, NES_OP_STA, (uint8_t)(destination->address + i));
        }
    }

    return true;
}

static bool emit_alloc_from_zp_var_to_code_var(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAlloc *source, ZnesAlloc *destination)
{
    if (source->type != destination->type)
        return false;

    if (source->type == ZNES_ALLOC_TYPE_ARRAY)
    {
        ZnesArrayAlloc *source_array = (ZnesArrayAlloc*) source;
        ZnesArrayAlloc *dest_array = (ZnesArrayAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_array->elements); i++)
            if (!emit_alloc_from_zp_var_to_code_var(program, segment, is_startup, source_array->elements[i], dest_array->elements[i]))
                return false;
    }
    else if (source->type == ZNES_ALLOC_TYPE_STRUCT)
    {
        ZnesStructAlloc *source_struct = (ZnesStructAlloc*) source;
        ZnesStructAlloc *dest_struct = (ZnesStructAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_struct->members); i++)
            if (!emit_alloc_from_zp_var_to_code_var(program, segment, is_startup, source_struct->members[i], dest_struct->members[i]))
                return false;
    }
    else
    {
        // FIXME: fix this suboptimal implementation
        size_t to_copy = source->size > destination->size 
                            ? destination->size 
                            : source->size;

        for (size_t i=0; i < to_copy; i++)
        {
            rp2a03_program_emit_zpg(program, segment, NES_OP_LDA, source->address + i);
            rp2a03_program_emit_abs(program, segment, NES_OP_STA, destination->address + i);
        }

        if (destination->size > source->size)
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < destination->size; i++)
                rp2a03_program_emit_abs(program, segment, NES_OP_STA, destination->address + i);
        }
    }

    return true;
}

static bool emit_alloc_from_zp_var_to_data_var(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAlloc *source, ZnesAlloc *destination)
{
    if (source->type != destination->type)
        return false;

    if (source->type == ZNES_ALLOC_TYPE_ARRAY)
    {
        ZnesArrayAlloc *source_array = (ZnesArrayAlloc*) source;
        ZnesArrayAlloc *dest_array = (ZnesArrayAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_array->elements); i++)
            if (!emit_alloc_from_zp_var_to_data_var(program, segment, is_startup, source_array->elements[i], dest_array->elements[i]))
                return false;
    }
    else if (source->type == ZNES_ALLOC_TYPE_STRUCT)
    {
        ZnesStructAlloc *source_struct = (ZnesStructAlloc*) source;
        ZnesStructAlloc *dest_struct = (ZnesStructAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_struct->members); i++)
            if (!emit_alloc_from_zp_var_to_data_var(program, segment, is_startup, source_struct->members[i], dest_struct->members[i]))
                return false;
    }
    else
    {
        // FIXME: fix this suboptimal implementation
        size_t to_copy = source->size > destination->size 
                            ? destination->size 
                            : source->size;

        for (size_t i=0; i < to_copy; i++)
        {
            rp2a03_program_emit_zpg(program, segment, NES_OP_LDA, source->address + i);
            rp2a03_program_emit_abs(program, segment, NES_OP_STA, destination->address + i);
        }

        if (destination->size > source->size)
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < destination->size; i++)
                rp2a03_program_emit_abs(program, segment, NES_OP_STA, destination->address + i);
        }
    }

    return true;
}

static bool emit_alloc_from_data_var_to_data_var(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAlloc *source, ZnesAlloc *destination)
{
    if (source->type != destination->type)
        return false;

    if (source->type == ZNES_ALLOC_TYPE_ARRAY)
    {
        ZnesArrayAlloc *source_array = (ZnesArrayAlloc*) source;
        ZnesArrayAlloc *dest_array = (ZnesArrayAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_array->elements); i++)
            if (!emit_alloc_from_data_var_to_data_var(program, segment, is_startup, source_array->elements[i], dest_array->elements[i]))
                return false;
    }
    else if (source->type == ZNES_ALLOC_TYPE_STRUCT)
    {
        ZnesStructAlloc *source_struct = (ZnesStructAlloc*) source;
        ZnesStructAlloc *dest_struct = (ZnesStructAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_struct->members); i++)
            if (!emit_alloc_from_data_var_to_data_var(program, segment, is_startup, source_struct->members[i], dest_struct->members[i]))
                return false;
    }
    else
    {
        if (is_startup)
        {
            // NOTE: In static context we can directly copy from one part of the DATA segment to another at compile time
            uint8_t *source_data_seg_slot = program->data->bytes + source->address - program->data->base_address;
            uint8_t *dest_data_seg_slot = program->data->bytes + (destination->address - program->data->base_address);

            size_t to_copy = source->size > destination->size
                                ? destination->size
                                : source->size;

            memcpy(dest_data_seg_slot, source_data_seg_slot, to_copy);
            if (destination->size > source->size)
                memset(dest_data_seg_slot + to_copy, 0, destination->size - to_copy);
        }
        else
        {
            // FIXME: fix this suboptimal implementation
            size_t to_copy = source->size > destination->size 
                                ? destination->size 
                                : source->size;

            for (size_t i=0; i < to_copy; i++)
            {
                rp2a03_program_emit_abs(program, segment, NES_OP_LDA, source->address + i);
                rp2a03_program_emit_abs(program, segment, NES_OP_STA, destination->address + i);
            }

            if (destination->size > source->size)
            {
                rp2a03_program_emit_imm(program, segment, NES_OP_LDA, 0x0);
                for (size_t i = to_copy; i < destination->size; i++)
                    rp2a03_program_emit_abs(program, segment, NES_OP_STA, destination->address + i);
            }
        }
    }

    return true;
}

static bool emit_alloc_from_data_var_to_zp_var(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAlloc *source, ZnesAlloc *destination)
{
    if (source->type != destination->type)
        return false;

    if (source->type == ZNES_ALLOC_TYPE_ARRAY)
    {
        ZnesArrayAlloc *source_array = (ZnesArrayAlloc*) source;
        ZnesArrayAlloc *dest_array = (ZnesArrayAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_array->elements); i++)
            if (!emit_alloc_from_data_var_to_zp_var(program, segment, is_startup, source_array->elements[i], dest_array->elements[i]))
                return false;
    }
    else if (source->type == ZNES_ALLOC_TYPE_STRUCT)
    {
        ZnesStructAlloc *source_struct = (ZnesStructAlloc*) source;
        ZnesStructAlloc *dest_struct = (ZnesStructAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_struct->members); i++)
            if (!emit_alloc_from_data_var_to_zp_var(program, segment, is_startup, source_struct->members[i], dest_struct->members[i]))
                return false;
    }
    else
    {
        // FIXME: fix this suboptimal implementation
        size_t to_copy = source->size > destination->size 
                            ? destination->size 
                            : source->size;

        for (size_t i=0; i < to_copy; i++)
        {
            if (is_startup)
            {
                // NOTE: In startup context we can directly copy the value from the DATA segment
                rp2a03_program_emit_imm(program, segment, NES_OP_LDA, program->data->bytes[source->address - program->data->base_address + i]);
            }
            else
            {
                // NOTE: If we are not in startup context, we need to get the current -possibly modified- value from the DATA segment
                rp2a03_program_emit_abs(program, segment, NES_OP_LDA, source->address + i);
            }
            rp2a03_program_emit_zpg(program, segment, NES_OP_STA, (uint8_t)(destination->address + i));
        }

        if (destination->size > source->size)
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < destination->size; i++)
                rp2a03_program_emit_zpg(program, segment, NES_OP_STA, (uint8_t)(destination->address + i));
        }
    }

    return true;
}

static bool emit_alloc_from_data_var_to_code_var(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAlloc *source, ZnesAlloc *destination)
{
    if (source->type != destination->type)
        return false;

    if (source->type == ZNES_ALLOC_TYPE_ARRAY)
    {
        ZnesArrayAlloc *source_array = (ZnesArrayAlloc*) source;
        ZnesArrayAlloc *dest_array = (ZnesArrayAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_array->elements); i++)
            if (!emit_alloc_from_data_var_to_code_var(program, segment, is_startup, source_array->elements[i], dest_array->elements[i]))
                return false;
    }
    else if (source->type == ZNES_ALLOC_TYPE_STRUCT)
    {
        ZnesStructAlloc *source_struct = (ZnesStructAlloc*) source;
        ZnesStructAlloc *dest_struct = (ZnesStructAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_struct->members); i++)
            if (!emit_alloc_from_data_var_to_code_var(program, segment, is_startup, source_struct->members[i], dest_struct->members[i]))
                return false;
    }
    else
    {
        // FIXME: fix this suboptimal implementation
        size_t to_copy = source->size > destination->size 
                            ? destination->size 
                            : source->size;

        for (size_t i=0; i < to_copy; i++)
        {
            rp2a03_program_emit_abs(program, segment, NES_OP_LDA, source->address + i);
            rp2a03_program_emit_abs(program, segment, NES_OP_STA, destination->address + i);
        }

        if (destination->size > source->size)
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < destination->size; i++)
                rp2a03_program_emit_abs(program, segment, NES_OP_STA, destination->address + i);
        }
    }

    return true;
}

static bool emit_alloc_from_code_var_to_code_var(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAlloc *source, ZnesAlloc *destination)
{
    if (source->type != destination->type)
        return false;

    if (source->type == ZNES_ALLOC_TYPE_ARRAY)
    {
        ZnesArrayAlloc *source_array = (ZnesArrayAlloc*) source;
        ZnesArrayAlloc *dest_array = (ZnesArrayAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_array->elements); i++)
            if (!emit_alloc_from_code_var_to_code_var(program, segment, is_startup, source_array->elements[i], dest_array->elements[i]))
                return false;
    }
    else if (source->type == ZNES_ALLOC_TYPE_STRUCT)
    {
        ZnesStructAlloc *source_struct = (ZnesStructAlloc*) source;
        ZnesStructAlloc *dest_struct = (ZnesStructAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_struct->members); i++)
            if (!emit_alloc_from_code_var_to_code_var(program, segment, is_startup, source_struct->members[i], dest_struct->members[i]))
                return false;
    }
    else
    {
        // FIXME: fix this suboptimal implementation
        size_t to_copy = source->size > destination->size 
                            ? destination->size 
                            : source->size;

        for (size_t i=0; i < to_copy; i++)
        {
            rp2a03_program_emit_abs(program, segment, NES_OP_LDA, source->address + i);
            rp2a03_program_emit_abs(program, segment, NES_OP_STA, destination->address + i);
        }

        if (destination->size > source->size)
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < destination->size; i++)
                rp2a03_program_emit_abs(program, segment, NES_OP_STA, destination->address + i);
        }
    }

    return true;
}

static bool emit_alloc_from_code_var_to_zp_var(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAlloc *source, ZnesAlloc *destination)
{
    if (source->type != destination->type)
        return false;

    if (source->type == ZNES_ALLOC_TYPE_ARRAY)
    {
        ZnesArrayAlloc *source_array = (ZnesArrayAlloc*) source;
        ZnesArrayAlloc *dest_array = (ZnesArrayAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_array->elements); i++)
            if (!emit_alloc_from_code_var_to_zp_var(program, segment, is_startup, source_array->elements[i], dest_array->elements[i]))
                return false;
    }
    else if (source->type == ZNES_ALLOC_TYPE_STRUCT)
    {
        ZnesStructAlloc *source_struct = (ZnesStructAlloc*) source;
        ZnesStructAlloc *dest_struct = (ZnesStructAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_struct->members); i++)
            if (!emit_alloc_from_code_var_to_zp_var(program, segment, is_startup, source_struct->members[i], dest_struct->members[i]))
                return false;
    }
    else
    {
        // FIXME: fix this suboptimal implementation
        size_t to_copy = source->size > destination->size 
                            ? destination->size 
                            : source->size;

        for (size_t i=0; i < to_copy; i++)
        {
            rp2a03_program_emit_abs(program, segment, NES_OP_LDA, source->address + i);
            rp2a03_program_emit_zpg(program, segment, NES_OP_STA, (uint8_t) (destination->address + i));
        }

        if (destination->size > source->size)
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < destination->size; i++)
                rp2a03_program_emit_zpg(program, segment, NES_OP_STA, (uint8_t) destination->address + i);
        }
    }

    return true;
}

static bool emit_alloc_from_code_var_to_data_var(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAlloc *source, ZnesAlloc *destination)
{
    if (source->type != destination->type)
        return false;

    if (source->type == ZNES_ALLOC_TYPE_ARRAY)
    {
        ZnesArrayAlloc *source_array = (ZnesArrayAlloc*) source;
        ZnesArrayAlloc *dest_array = (ZnesArrayAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_array->elements); i++)
            if (!emit_alloc_from_code_var_to_data_var(program, segment, is_startup, source_array->elements[i], dest_array->elements[i]))
                return false;
    }
    else if (source->type == ZNES_ALLOC_TYPE_STRUCT)
    {
        ZnesStructAlloc *source_struct = (ZnesStructAlloc*) source;
        ZnesStructAlloc *dest_struct = (ZnesStructAlloc*) destination;

        for (size_t i=0; i < fl_array_length(dest_struct->members); i++)
            if (!emit_alloc_from_code_var_to_data_var(program, segment, is_startup, source_struct->members[i], dest_struct->members[i]))
                return false;
    }
    else
    {
        // FIXME: fix this suboptimal implementation
        size_t to_copy = source->size > destination->size 
                            ? destination->size 
                            : source->size;

        for (size_t i=0; i < to_copy; i++)
        {
            rp2a03_program_emit_abs(program, segment, NES_OP_LDA, source->address + i);
            rp2a03_program_emit_abs(program, segment, NES_OP_STA, destination->address + i);
        }

        if (destination->size > source->size)
        {
            rp2a03_program_emit_imm(program, segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < destination->size; i++)
                rp2a03_program_emit_abs(program, segment, NES_OP_STA, destination->address + i);
        }
    }

    return true;
}

static inline bool emit_alloc_from_variable(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesAllocInstruction *instruction)
{
    ZnesAlloc *source_variable = ((ZnesVariableOperand*) instruction->source)->variable;

    // If the source is a temp symbol, we need to "emit" the store in a special way
    if (source_variable->type == ZNES_ALLOC_TYPE_TEMP)
    {
        ZnesAllocInstruction temp_alloc = znes_alloc_instruction_new_local(instruction->destination, ((ZnesTempAlloc*) source_variable)->source);
        return rp2a03_emit_alloc_instruction(program, segment, is_startup, &temp_alloc);
    }

    if (instruction->destination->segment == ZNES_SEGMENT_ZP)
    {
        if (source_variable->segment == ZNES_SEGMENT_ZP)
            return emit_alloc_from_zp_var_to_zp_var(program, segment, is_startup, source_variable, instruction->destination);

        if (source_variable->segment == ZNES_SEGMENT_TEXT)
            return emit_alloc_from_code_var_to_zp_var(program, segment, is_startup, source_variable, instruction->destination);
        
        if (source_variable->segment == ZNES_SEGMENT_DATA)
            return emit_alloc_from_data_var_to_zp_var(program, segment, is_startup, source_variable, instruction->destination);
    }
    else if (instruction->destination->segment == ZNES_SEGMENT_DATA)
    {
        if (source_variable->segment == ZNES_SEGMENT_DATA)
            return emit_alloc_from_data_var_to_data_var(program, segment, is_startup, source_variable, instruction->destination);
        
        if (source_variable->segment == ZNES_SEGMENT_ZP)
            return emit_alloc_from_zp_var_to_data_var(program, segment, is_startup, source_variable, instruction->destination);
        
        if (source_variable->segment == ZNES_SEGMENT_TEXT)
            return emit_alloc_from_code_var_to_data_var(program, segment, is_startup, source_variable, instruction->destination);
    }
    else if (instruction->destination->segment == ZNES_SEGMENT_TEXT)
    {
        if (source_variable->segment == ZNES_SEGMENT_TEXT)
            return emit_alloc_from_code_var_to_code_var(program, segment, is_startup, source_variable, instruction->destination);

        if (source_variable->segment == ZNES_SEGMENT_ZP)
            return emit_alloc_from_zp_var_to_code_var(program, segment, is_startup, source_variable, instruction->destination);
        
        if (source_variable->segment == ZNES_SEGMENT_DATA)
            return emit_alloc_from_data_var_to_code_var(program, segment, is_startup, source_variable, instruction->destination);
    }

    return false;
}

#endif /* RP2A03_EMIT_ALLOC_VAR_H */
