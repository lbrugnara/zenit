#include "emit.h"

void zenit_nes_emit_store_array(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, size_t offset, size_t gap, struct ZirArrayOperand *array)
{
    for (size_t i=0; i < fl_array_length(array->elements); i++)
    {
        struct ZirOperand *operand = array->elements[i];
        
        if (operand->type == ZIR_OPERAND_UINT)
        {
            struct ZirUintOperand *uint_elem = (struct ZirUintOperand*)operand;
            zenit_nes_emit_store_uint(program, nes_symbol, offset + (i * gap), uint_elem);
        }
        else if (operand->type == ZIR_OPERAND_ARRAY)
        {
            struct ZirArrayOperand *array_elem = (struct ZirArrayOperand*)operand;
            zenit_nes_emit_store_array(program, nes_symbol, offset + (i * gap), zir_type_size(array_elem->typeinfo->member_type), array_elem);
        }
        else if (operand->type == ZIR_OPERAND_REFERENCE)
        {
            struct ZirReferenceOperand *ref_elem = (struct ZirReferenceOperand*)operand;
            zenit_nes_emit_store_reference(program, nes_symbol, offset + (i * gap), ref_elem);
        }
        else if (operand->type == ZIR_OPERAND_SYMBOL)
        {
            struct ZirSymbolOperand *symbol_elem = (struct ZirSymbolOperand*)operand;
            zenit_nes_emit_store_symbol(program, nes_symbol, offset + (i * gap), symbol_elem);
        }
    }
}

void zenit_nes_emit_store_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, size_t offset, struct ZirSymbolOperand *symbol_operand)
{
    uint16_t target_address = nes_symbol->address + offset;
    struct ZenitNesSymbol *source_symbol = fl_hashtable_get(program->symbols, symbol_operand->symbol->name);

    if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP && source_symbol->segment == ZENIT_NES_SEGMENT_ZP)
    {
        // FIXME: fix this suboptimal implementation
        size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
                ? nes_symbol->element_size 
                : source_symbol->element_size;

        for (size_t i=0; i < to_copy; i++)
        {
            // FIXME: Select startup or code based on the program current's block
            zenit_nes_program_emit_zpg(&program->startup, NES_OP_LDA, (uint8_t)(source_symbol->address + i));
            zenit_nes_program_emit_zpg(&program->startup, NES_OP_STA, (uint8_t)(target_address + i));
        }

        if (nes_symbol->element_size > source_symbol->element_size)
        {
            // FIXME: Select startup or code based on the program current's block
            zenit_nes_program_emit_imm(&program->startup, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < nes_symbol->element_size; i++)
                zenit_nes_program_emit_zpg(&program->startup, NES_OP_STA, (uint8_t)(target_address + i));
        }
        return;
    }
    if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP)
    {
        if (source_symbol->segment == ZENIT_NES_SEGMENT_CODE)
        {
            // FIXME: Handle the CODE segment
        }
        else if (source_symbol->segment == ZENIT_NES_SEGMENT_DATA)
        {
            // FIXME: fix this suboptimal implementation
            size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
                    ? nes_symbol->element_size 
                    : source_symbol->element_size;

            for (size_t i=0; i < to_copy; i++)
            {
                // FIXME: Select startup or code based on the program current's block
                zenit_nes_program_emit_imm(&program->startup, NES_OP_LDA, *((program->data.bytes + source_symbol->address - program->data.base_address) + i));
                zenit_nes_program_emit_zpg(&program->startup, NES_OP_STA, (uint8_t)(target_address + i));
            }

            if (nes_symbol->element_size > source_symbol->element_size)
            {
                // FIXME: Select startup or code based on the program current's block
                zenit_nes_program_emit_imm(&program->startup, NES_OP_LDA, 0x0);
                for (size_t i = to_copy; i < nes_symbol->element_size; i++)
                    zenit_nes_program_emit_zpg(&program->startup, NES_OP_STA, (uint8_t)(target_address + i));
            }
        }
        else if (source_symbol->segment == ZENIT_NES_SEGMENT_TMP)
        {
            // FIXME: fix this suboptimal implementation
            size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
                    ? nes_symbol->element_size 
                    : source_symbol->element_size;

            for (size_t i=0; i < to_copy; i++)
            {
                // FIXME: Select startup or code based on the program current's block
                zenit_nes_program_emit_imm(&program->startup, NES_OP_LDA, *(program->tmp.bytes + source_symbol->address + i));
                zenit_nes_program_emit_zpg(&program->startup, NES_OP_STA, (uint8_t)(target_address + i));
            }

            if (nes_symbol->element_size > source_symbol->element_size)
            {
                // FIXME: Select startup or code based on the program current's block
                zenit_nes_program_emit_imm(&program->startup, NES_OP_LDA, 0x0);
                for (size_t i = to_copy; i < nes_symbol->element_size; i++)
                {
                    zenit_nes_program_emit_zpg(&program->startup, NES_OP_STA, (uint8_t)(target_address + i));
                }
            }
        }

        return;
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
    {
        // FIXME: Handle the CODE segment
        return;
    }
    else if (source_symbol->segment == ZENIT_NES_SEGMENT_ZP)
    {
        if (nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
        {
            // FIXME: Handle the CODE segment
        }
        else if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
        {
            // FIXME: fix this suboptimal implementation
            size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
                    ? nes_symbol->element_size 
                    : source_symbol->element_size;

            for (size_t i=0; i < to_copy; i++)
            {
                // FIXME: Select startup or code based on the program current's block
                zenit_nes_program_emit_zpg(&program->startup, NES_OP_LDA, (uint8_t)(source_symbol->address + i));
                zenit_nes_program_emit_abs(&program->startup, NES_OP_STA, target_address + i);
            }

            if (nes_symbol->element_size > source_symbol->element_size)
            {
                // FIXME: Select startup or code based on the program current's block
                zenit_nes_program_emit_imm(&program->startup, NES_OP_LDA, 0x0);
                for (size_t i = to_copy; i < nes_symbol->element_size; i++)
                    zenit_nes_program_emit_abs(&program->startup, NES_OP_STA, target_address + i);
            }
        }
        else if (nes_symbol->segment == ZENIT_NES_SEGMENT_TMP)
        {
            // FIXME: handle copy from ZP to TMP (TMP is a compile time concept)
        }

        return;
    }
    else if (source_symbol->segment == ZENIT_NES_SEGMENT_CODE)
    {
        // FIXME: Handle the CODE segment
        return;
    }
    
    // NOTE: At this point we are handling a store within the DATA segment or between the DATA segment and our TMP segment
    // We need to know from where we should get the source values and which is the destination slot
    uint8_t *source = NULL;
    uint8_t *destination = NULL;

    if (source_symbol->segment == ZENIT_NES_SEGMENT_TMP)
    {
        source = program->tmp.bytes + source_symbol->address;
    }
    else if (source_symbol->segment == ZENIT_NES_SEGMENT_DATA)
    {
        source = program->data.bytes + source_symbol->address - program->data.base_address;
    }

    if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
    {
        destination = program->data.bytes + (target_address - program->data.base_address);
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_TMP)
    {
        destination = program->tmp.bytes + target_address;
    }

    if (source == NULL || destination == NULL)
    {
        // FIXME: Add an error
        return;
    }

    // Process the copy, including checks for down casting or up casting
    for (size_t i=0, j=0; j < source_symbol->elements; i++, j++)
    {
        size_t to_copy = source_symbol->element_size > nes_symbol->element_size 
            ? nes_symbol->element_size 
            : source_symbol->element_size;

        memcpy(destination + nes_symbol->element_size * i, source + source_symbol->element_size * j, to_copy);
    }
}

void zenit_nes_emit_store_reference(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, size_t offset, struct ZirReferenceOperand *reference_operand)
{
    uint16_t target_address = nes_symbol->address + offset;
    struct ZenitNesSymbol *ref_op_symbol = fl_hashtable_get(program->symbols, reference_operand->operand->symbol->name);

    if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP)
    {
        // FIXME: Select startup or code based on the program current's block
        zenit_nes_program_emit_imm(&program->startup, NES_OP_LDA, (uint8_t)(ref_op_symbol->address & 0xFF));
        zenit_nes_program_emit_imm(&program->startup, NES_OP_LDX, (uint8_t)((ref_op_symbol->address >> 8) & 0xFF));
        zenit_nes_program_emit_zpg(&program->startup, NES_OP_STA, (uint8_t)(target_address));
        zenit_nes_program_emit_zpg(&program->startup, NES_OP_STX, (uint8_t)(target_address + 1));
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
    {
        // FIXME: handle the CODE segment
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
    {
        uint8_t *slot = program->data.bytes + (target_address - program->data.base_address);
        *slot       = ref_op_symbol->address & 0xFF;
        *(slot+1)   = (ref_op_symbol->address >> 8) & 0xFF;
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_TMP)
    {
        uint8_t *slot = program->tmp.bytes + target_address;
        *slot       = ref_op_symbol->address & 0xFF;
        *(slot+1)   = (ref_op_symbol->address >> 8) & 0xFF;
    }
}

void zenit_nes_emit_store_uint(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, size_t offset, struct ZirUintOperand *uint_operand)
{
    uint16_t target_address = nes_symbol->address + offset;

    if (uint_operand->typeinfo->size == ZIR_UINT_8)
    {
        if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP)
        {
            // FIXME: Select startup or code based on the program current's block
            zenit_nes_program_emit_imm(&program->startup, NES_OP_LDA, uint_operand->value.uint8);
            zenit_nes_program_emit_zpg(&program->startup, NES_OP_STA, (uint8_t)(target_address));

            // If destination is greater than uint16
            if (nes_symbol->element_size > 1 /*byte*/)
            {
                zenit_nes_program_emit_imm(&program->startup, NES_OP_LDX, 0x0);
                size_t to_fill = nes_symbol->element_size - 1;
                for (size_t i=0; i < to_fill; i++)
                    zenit_nes_program_emit_zpg(&program->startup, NES_OP_STX, (uint8_t)(target_address + i + 1));
            }
        }
        else if (nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
        {
            // FIXME: handle the CODE segment
        }
        else if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
        {
            uint8_t *slot = program->data.bytes + (target_address - program->data.base_address);
            *slot = uint_operand->value.uint8;
            
            // upcast from uint8 to uint16
            if (nes_symbol->element_size > 1 /*byte*/)
            {
                slot++;
                size_t to_fill = nes_symbol->element_size - 1;
                for (size_t i=0; i < to_fill; i++)
                    *(slot + i) = 0;
            }
        }
        else if (nes_symbol->segment == ZENIT_NES_SEGMENT_TMP)
        {
            uint8_t *slot = program->tmp.bytes + target_address;
            *slot = uint_operand->value.uint8;
            
            // upcast from uint8 to uint16
            if (nes_symbol->element_size > 1 /*byte*/)
            {
                slot++;
                size_t to_fill = nes_symbol->element_size - 1;
                for (size_t i=0; i < to_fill; i++)
                    *(slot + i) = 0;
            }
        }
    }
    else if (uint_operand->typeinfo->size == ZIR_UINT_16)
    {
        if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP)
        {
            // FIXME: Select startup or code based on the program current's block
            zenit_nes_program_emit_imm(&program->startup, NES_OP_LDA, (uint8_t)(uint_operand->value.uint16 & 0xFF));
            zenit_nes_program_emit_zpg(&program->startup, NES_OP_STA, (uint8_t)(target_address));

            if (nes_symbol->element_size >= 2 /*bytes*/)
            {
                zenit_nes_program_emit_imm(&program->startup, NES_OP_LDX, (uint8_t)((uint_operand->value.uint16 >> 8) & 0xFF));
                zenit_nes_program_emit_zpg(&program->startup, NES_OP_STX, (uint8_t)(target_address + 1));
            }

            // If destination is greater than uint16
            if (nes_symbol->element_size > 2 /*bytes*/)
            {
                zenit_nes_program_emit_imm(&program->startup, NES_OP_LDX, 0x0);
                size_t to_fill = nes_symbol->element_size - 2;
                for (size_t i=0; i < to_fill; i++)
                    zenit_nes_program_emit_zpg(&program->startup, NES_OP_STX, (uint8_t)(target_address + i + 2));
            }
        }
        else if (nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
        {
            // FIXME: handle the CODE segment
        }
        else if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
        {
            uint8_t *slot = program->data.bytes + (target_address - program->data.base_address);
            *slot = uint_operand->value.uint16 & 0xFF;
            
            if (nes_symbol->element_size >= 2 /*bytes*/)
                *(slot+1)   = (uint_operand->value.uint16 >> 8) & 0xFF;

            // If destination is greater than uint16
            if (nes_symbol->element_size > 2)
            {
                slot += 2;
                size_t to_fill = nes_symbol->element_size - 2;
                for (size_t i=0; i < to_fill; i++)
                    *(slot + i) = 0;
            }
        }
        else if (nes_symbol->segment == ZENIT_NES_SEGMENT_TMP)
        {
            uint8_t *slot = program->tmp.bytes + target_address;
            *slot = uint_operand->value.uint16 & 0xFF;
            
            if (nes_symbol->element_size == 2 /*bytes*/)
                *(slot+1) = (uint_operand->value.uint16 >> 8) & 0xFF;

            // If destination is greater than uint16
            if (nes_symbol->element_size > 2)
            {
                slot += 2;
                size_t to_fill = nes_symbol->element_size - 2;
                for (size_t i=0; i < to_fill; i++)
                    *(slot + i) = 0;
            }
        }
    }
}
