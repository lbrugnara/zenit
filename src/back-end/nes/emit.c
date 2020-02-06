#include "emit.h"

void zenit_nes_emit_store_array(struct ZenitNesProgram *program, struct ZirArrayOperand *array, struct ZenitNesSymbol *nes_symbol, size_t offset, size_t gap)
{
    // The store array on a temp symbol is just the assignment of the array operand to the "source" property of the temp symbol
    if (nes_symbol->segment == ZENIT_NES_SEGMENT_TEMP)
    {
        ((struct ZenitNesTmpSymbol*) nes_symbol)->source = (struct ZirOperand*) array;
        return;
    }

    for (size_t i=0; i < fl_array_length(array->elements); i++)
    {
        struct ZirOperand *operand = array->elements[i];
        
        if (operand->type == ZIR_OPERAND_UINT)
        {
            zenit_nes_emit_store_uint(program, (struct ZirUintOperand*) operand, nes_symbol, offset + (i * gap));
        }
        else if (operand->type == ZIR_OPERAND_ARRAY)
        {
            struct ZirArrayOperand *array_elem = (struct ZirArrayOperand*)operand;
            zenit_nes_emit_store_array(program, array_elem, nes_symbol, offset + (i * gap), zir_type_size(array_elem->type->member_type));
        }
        else if (operand->type == ZIR_OPERAND_STRUCT)
        {
            zenit_nes_emit_store_struct(program, (struct ZirStructOperand*) operand, nes_symbol, offset + (i * gap));
        }
        else if (operand->type == ZIR_OPERAND_REFERENCE)
        {
            zenit_nes_emit_store_reference(program, (struct ZirReferenceOperand*) operand, nes_symbol, offset + (i * gap));
        }
        else if (operand->type == ZIR_OPERAND_SYMBOL)
        {
            zenit_nes_emit_store_symbol(program, (struct ZirSymbolOperand*) operand, nes_symbol, offset + (i * gap));
        }
    }
}

void zenit_nes_emit_store_struct(struct ZenitNesProgram *program, struct ZirStructOperand *struct_operand, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    size_t gap = 0;
    for (size_t i=0; i < fl_array_length(struct_operand->members); i++)
    {
        struct ZirStructOperandMember *operand_member = struct_operand->members[i];
        struct ZirStructTypeMember *type_member = zir_type_struct_get_member(struct_operand->type, operand_member->name);

        if (operand_member->operand->type == ZIR_OPERAND_UINT)
        {
            zenit_nes_emit_store_uint(program, (struct ZirUintOperand*) operand_member->operand, nes_symbol, offset + (i * gap));
        }
        else if (operand_member->operand->type == ZIR_OPERAND_ARRAY)
        {
            struct ZirArrayOperand *array_elem = (struct ZirArrayOperand*)operand_member->operand;
            zenit_nes_emit_store_array(program, array_elem, nes_symbol, offset + (i * gap), zir_type_size(array_elem->type->member_type));
        }
        else if (operand_member->operand->type == ZIR_OPERAND_STRUCT)
        {
            zenit_nes_emit_store_struct(program, (struct ZirStructOperand*) operand_member->operand, nes_symbol, offset + (i * gap));
        }
        else if (operand_member->operand->type == ZIR_OPERAND_REFERENCE)
        {
            zenit_nes_emit_store_reference(program, (struct ZirReferenceOperand*) operand_member->operand, nes_symbol, offset + (i * gap));
        }
        else if (operand_member->operand->type == ZIR_OPERAND_SYMBOL)
        {
            zenit_nes_emit_store_symbol(program, (struct ZirSymbolOperand*) operand_member->operand, nes_symbol, offset + (i * gap));
        }

        gap += zir_type_size(type_member->type);
    }
}

void zenit_nes_emit_store_temp(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, size_t offset, struct ZenitNesTmpSymbol *temp_symbol)
{
    if (temp_symbol->source->type == ZIR_OPERAND_ARRAY)
    {
        size_t array_size = zir_type_size(((struct ZirArrayOperand*) temp_symbol->source)->type->member_type);
        size_t gap = nes_symbol->element_size > array_size ? array_size : nes_symbol->element_size;
        zenit_nes_emit_store_array(program, (struct ZirArrayOperand*) temp_symbol->source, nes_symbol, offset, gap);
    }
    else if (temp_symbol->source->type == ZIR_OPERAND_STRUCT)
    {
        zenit_nes_emit_store_struct(program, (struct ZirStructOperand*) temp_symbol->source, nes_symbol, offset);
    }
    else if (temp_symbol->source->type == ZIR_OPERAND_REFERENCE)
    {
        zenit_nes_emit_store_reference(program, (struct ZirReferenceOperand*) temp_symbol->source, nes_symbol, offset);
    }
    else if (temp_symbol->source->type == ZIR_OPERAND_SYMBOL)
    {
        zenit_nes_emit_store_symbol(program, (struct ZirSymbolOperand*) temp_symbol->source, nes_symbol, offset);
    }
    else if (temp_symbol->source->type == ZIR_OPERAND_UINT)
    {
        zenit_nes_emit_store_uint(program, (struct ZirUintOperand*) temp_symbol->source, nes_symbol, offset);
    }
}

void zenit_nes_emit_store_symbol(struct ZenitNesProgram *program, struct ZirSymbolOperand *symbol_operand, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    // The store symbol on a temp symbol is just the assignment of the symbol operand to the "source" property of the temp symbol
    if (nes_symbol->segment == ZENIT_NES_SEGMENT_TEMP)
    {
        ((struct ZenitNesTmpSymbol*) nes_symbol)->source = (struct ZirOperand*) symbol_operand;
        return;
    }

    struct ZenitNesSymbol *source_symbol = fl_hashtable_get(program->symbols, symbol_operand->symbol->name);

    // If the source is a temp symbol, we need to "emit" the store in a special way
    if (source_symbol->segment == ZENIT_NES_SEGMENT_TEMP)
    {
        zenit_nes_emit_store_temp(program, nes_symbol, offset, (struct ZenitNesTmpSymbol*) source_symbol);
        return;
    }

    uint16_t target_address = nes_symbol->address + offset;

    if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP)
    {
        struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;

        if (source_symbol->segment == ZENIT_NES_SEGMENT_ZP)
        {
            // FIXME: fix this suboptimal implementation
            size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
                    ? nes_symbol->element_size 
                    : source_symbol->element_size;

            for (size_t i=0; i < to_copy; i++)
            {
                zenit_nes_program_emit_zpg(target_segment, NES_OP_LDA, (uint8_t)(source_symbol->address + i));
                zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address + i));
            }

            if (nes_symbol->element_size > source_symbol->element_size)
            {
                zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
                for (size_t i = to_copy; i < nes_symbol->element_size; i++)
                    zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address + i));
            }
        }
        else if (source_symbol->segment == ZENIT_NES_SEGMENT_CODE)
        {
            // FIXME: fix this suboptimal implementation
            size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
                    ? nes_symbol->element_size 
                    : source_symbol->element_size;

            for (size_t i=0; i < to_copy; i++)
            {
                zenit_nes_program_emit_abs(target_segment, NES_OP_LDA, source_symbol->address + i);
                zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address + i));
            }

            if (nes_symbol->element_size > source_symbol->element_size)
            {
                zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
                for (size_t i = to_copy; i < nes_symbol->element_size; i++)
                    zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address + i));
            }
        }
        else if (source_symbol->segment == ZENIT_NES_SEGMENT_DATA)
        {
            // FIXME: fix this suboptimal implementation
            size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
                    ? nes_symbol->element_size 
                    : source_symbol->element_size;

            for (size_t i=0; i < to_copy; i++)
            {
                if (program->static_context)
                {
                    // NOTE: In static context we can directly copy the value from the DATA segment
                    zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, program->data.bytes[source_symbol->address - program->data.base_address + i]);
                }
                else
                {
                    // NOTE: If we are not in static context, we need to get the current -possibly modified- value from the DATA segment
                    zenit_nes_program_emit_abs(target_segment, NES_OP_LDA, source_symbol->address + i);
                }

                zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address + i));
            }

            if (nes_symbol->element_size > source_symbol->element_size)
            {
                zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
                for (size_t i = to_copy; i < nes_symbol->element_size; i++)
                    zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address + i));
            }
        }
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
    {
        struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;

        if (source_symbol->segment == ZENIT_NES_SEGMENT_DATA)
        {
            if (program->static_context)
            {
                // NOTE: In static context we can directly copy from one part of the DATA segment to another at compile time
                uint8_t *source = program->data.bytes + source_symbol->address - program->data.base_address;
                uint8_t *destination = program->data.bytes + (target_address - program->data.base_address);

                // Process the copy, including checks for down casting or up casting
                for (size_t i=0, j=0; j < source_symbol->elements; i++, j++)
                {
                    size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
                        ? nes_symbol->element_size 
                        : source_symbol->element_size;

                    memcpy(destination + nes_symbol->element_size * i, source + source_symbol->element_size * j, to_copy);
                }
            }
            else
            {
                // NOTE: If we are not in static context, we need to copy from one part to the other of th DATA segment using
                // instructions
                // FIXME: fix this suboptimal implementation
                size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
                        ? nes_symbol->element_size 
                        : source_symbol->element_size;

                for (size_t i=0; i < to_copy; i++)
                {
                    zenit_nes_program_emit_abs(target_segment, NES_OP_LDA, source_symbol->address + i);
                    zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
                }

                if (nes_symbol->element_size > source_symbol->element_size)
                {
                    zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
                    for (size_t i = to_copy; i < nes_symbol->element_size; i++)
                        zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
                }
            }
        }
        else if (source_symbol->segment == ZENIT_NES_SEGMENT_ZP)
        {
            // FIXME: fix this suboptimal implementation
            size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
                    ? nes_symbol->element_size 
                    : source_symbol->element_size;

            for (size_t i=0; i < to_copy; i++)
            {
                zenit_nes_program_emit_zpg(target_segment, NES_OP_LDA, (uint8_t)(source_symbol->address + i));
                zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
            }

            if (nes_symbol->element_size > source_symbol->element_size)
            {
                zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
                for (size_t i = to_copy; i < nes_symbol->element_size; i++)
                    zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
            }
        }
        else if (source_symbol->segment == ZENIT_NES_SEGMENT_CODE)
        {
            // FIXME: fix this suboptimal implementation
            size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
                    ? nes_symbol->element_size 
                    : source_symbol->element_size;

            for (size_t i=0; i < to_copy; i++)
            {
                zenit_nes_program_emit_abs(target_segment, NES_OP_LDA, source_symbol->address + i);
                zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
            }

            if (nes_symbol->element_size > source_symbol->element_size)
            {
                zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
                for (size_t i = to_copy; i < nes_symbol->element_size; i++)
                    zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
            }
        }
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
    {
        struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;

        if (source_symbol->segment == ZENIT_NES_SEGMENT_CODE)
        {
            // FIXME: fix this suboptimal implementation
            size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
                    ? nes_symbol->element_size 
                    : source_symbol->element_size;

            for (size_t i=0; i < to_copy; i++)
            {
                zenit_nes_program_emit_abs(target_segment, NES_OP_LDA, source_symbol->address + i);
                zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
            }

            if (nes_symbol->element_size > source_symbol->element_size)
            {
                zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
                for (size_t i = to_copy; i < nes_symbol->element_size; i++)
                    zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
            }
        }
        else if (source_symbol->segment == ZENIT_NES_SEGMENT_ZP)
        {
            // FIXME: fix this suboptimal implementation
            size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
                    ? nes_symbol->element_size 
                    : source_symbol->element_size;

            for (size_t i=0; i < to_copy; i++)
            {
                zenit_nes_program_emit_zpg(target_segment, NES_OP_LDA, (uint8_t)(source_symbol->address + i));
                zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
            }

            if (nes_symbol->element_size > source_symbol->element_size)
            {
                zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
                for (size_t i = to_copy; i < nes_symbol->element_size; i++)
                    zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
            }
        }
        else if (source_symbol->segment == ZENIT_NES_SEGMENT_DATA)
        {
            // FIXME: fix this suboptimal implementation
            size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
                    ? nes_symbol->element_size 
                    : source_symbol->element_size;

            for (size_t i=0; i < to_copy; i++)
            {
                zenit_nes_program_emit_abs(target_segment, NES_OP_LDA, source_symbol->address + i);
                zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
            }

            if (nes_symbol->element_size > source_symbol->element_size)
            {
                zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
                for (size_t i = to_copy; i < nes_symbol->element_size; i++)
                    zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
            }
        }
    }
}

void zenit_nes_emit_store_reference(struct ZenitNesProgram *program, struct ZirReferenceOperand *reference_operand, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    // The store reference on a temp symbol is just the assignment of the reference operand to the "source" property of the temp symbol
    if (nes_symbol->segment == ZENIT_NES_SEGMENT_TEMP)
    {
        ((struct ZenitNesTmpSymbol*) nes_symbol)->source = (struct ZirOperand*) reference_operand;
        return;
    }

    uint16_t target_address = nes_symbol->address + offset;
    struct ZenitNesSymbol *ref_op_symbol = fl_hashtable_get(program->symbols, reference_operand->operand->symbol->name);

    if (ref_op_symbol->segment == ZENIT_NES_SEGMENT_TEMP)
        ref_op_symbol = zenit_nes_program_get_tmpsym_symbol(program, ref_op_symbol);

    if (ref_op_symbol == NULL)
        return;

    struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;

    if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP)
    {
        zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(ref_op_symbol->address & 0xFF));
        zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((ref_op_symbol->address >> 8) & 0xFF));
        zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address));
        zenit_nes_program_emit_zpg(target_segment, NES_OP_STX, (uint8_t)(target_address + 1));
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
    {
        zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(ref_op_symbol->address & 0xFF));
        zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((ref_op_symbol->address >> 8) & 0xFF));
        zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address);
        zenit_nes_program_emit_abs(target_segment, NES_OP_STX, target_address + 1);
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
    {
        if (program->static_context)
        {
            uint8_t *slot = program->data.bytes + (target_address - program->data.base_address);
            *slot       = ref_op_symbol->address & 0xFF;
            *(slot+1)   = (ref_op_symbol->address >> 8) & 0xFF;
        }
        else
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(ref_op_symbol->address & 0xFF));
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((ref_op_symbol->address >> 8) & 0xFF));
            zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address);
            zenit_nes_program_emit_abs(target_segment, NES_OP_STX, target_address + 1);
        }
    }
}

void zenit_nes_emit_store_uint(struct ZenitNesProgram *program, struct ZirUintOperand *uint_operand, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    // The store uint on a temp symbol is just the assignment of the uint operand to the "source" property of the temp symbol
    if (nes_symbol->segment == ZENIT_NES_SEGMENT_TEMP)
    {
        ((struct ZenitNesTmpSymbol*) nes_symbol)->source = (struct ZirOperand*) uint_operand;
        return;
    }

    uint16_t target_address = nes_symbol->address + offset;

    struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;

    // NOTE: We use a uint16_t because it can hold all the current uint values
    uint16_t uint_value = 0;

    if (uint_operand->type->size == ZIR_UINT_8)
    {
        uint_value = uint_operand->value.uint8;
    }
    else if (uint_operand->type->size == ZIR_UINT_16)
    {
        uint_value = uint_operand->value.uint16;
    }
    else
    {
        // FIXME: Add an error
        return;
    }

    if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP)
    {
        zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(uint_value & 0xFF));
        zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address));

        // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
        // uint_value (if it is uint8_t they will be all 0)
        if (nes_symbol->element_size >= 2 /*bytes*/)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((uint_value >> 8) & 0xFF));
            zenit_nes_program_emit_zpg(target_segment, NES_OP_STX, (uint8_t)(target_address + 1));
        }

        // If destination is greater than uint16, fill the destination with 0s
        if (nes_symbol->element_size > 2 /*bytes*/)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, 0x0);
            size_t to_fill = nes_symbol->element_size - 2;
            for (size_t i=0; i < to_fill; i++)
                zenit_nes_program_emit_zpg(target_segment, NES_OP_STX, (uint8_t)(target_address + i + 2));
        }
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
    {
        zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(uint_value & 0xFF));
        zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address);

        // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
        // uint_value (if it is uint8_t they will be all 0)
        if (nes_symbol->element_size >= 2 /*bytes*/)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((uint_value >> 8) & 0xFF));
            zenit_nes_program_emit_abs(target_segment, NES_OP_STX, target_address + 1);
        }

        // If destination is greater than uint16, fill the destination with 0s
        if (nes_symbol->element_size > 2 /*bytes*/)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, 0x0);
            size_t to_fill = nes_symbol->element_size - 2;
            for (size_t i=0; i < to_fill; i++)
                zenit_nes_program_emit_abs(target_segment, NES_OP_STX, target_address + i + 2);
        }
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
    {
        if (program->static_context)
        {
            uint8_t *slot = program->data.bytes + (target_address - program->data.base_address);
            *slot = uint_value & 0xFF;
            
            // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
            // uint_value (if it is uint8_t they will be all 0)
            if (nes_symbol->element_size >= 2 /*bytes*/)
                *(slot+1)   = (uint_value >> 8) & 0xFF;

            // If destination is greater than uint16, fill the destination with 0s
            if (nes_symbol->element_size > 2)
            {
                slot += 2;
                size_t to_fill = nes_symbol->element_size - 2;
                for (size_t i=0; i < to_fill; i++)
                    *(slot + i) = 0;
            }
        }
        else
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, (uint8_t)(uint_value & 0xFF));
            zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address);

            // If the element size is greater than or equals to 2, we need to store the highest 8 bits of the
            // uint_value (if it is uint8_t they will be all 0)
            if (nes_symbol->element_size >= 2 /*bytes*/)
            {
                zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, (uint8_t)((uint_value >> 8) & 0xFF));
                zenit_nes_program_emit_abs(target_segment, NES_OP_STX, target_address + 1);
            }

            // If destination is greater than uint16, fill the destination with 0s
            if (nes_symbol->element_size > 2 /*bytes*/)
            {
                zenit_nes_program_emit_imm(target_segment, NES_OP_LDX, 0x0);
                size_t to_fill = nes_symbol->element_size - 2;
                for (size_t i=0; i < to_fill; i++)
                    zenit_nes_program_emit_abs(target_segment, NES_OP_STX, target_address + i + 2);
            }
        }
    }
}
