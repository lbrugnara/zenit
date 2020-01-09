#include "emit.h"

void zenit_nes_emit_store_array(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, size_t offset, struct ZirArrayOperand *array)
{
    if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
    {
        // If the symbol is in the data segment, we just assign the value
        for (size_t i=0; i < fl_array_length(array->elements); i++)
        {
            struct ZirOperand *operand = array->elements[i];
            
            if (operand->type == ZIR_OPERAND_UINT)
            {
                struct ZirUintOperand *uint_elem = (struct ZirUintOperand*)operand;
                zenit_nes_emit_store_uint(program, nes_symbol, offset + (i * zir_type_uint_size(uint_elem->typeinfo)), uint_elem);
            }
            else if (operand->type == ZIR_OPERAND_ARRAY)
            {
                struct ZirArrayOperand *array_elem = (struct ZirArrayOperand*)operand;
                zenit_nes_emit_store_array(program, nes_symbol, offset + (i * zir_type_array_size(array_elem->typeinfo)), array_elem);
            }
            else if (operand->type == ZIR_OPERAND_REFERENCE)
            {
                struct ZirReferenceOperand *ref_elem = (struct ZirReferenceOperand*)operand;
                zenit_nes_emit_store_reference(program, nes_symbol, offset + (i * zir_type_reference_size(ref_elem->typeinfo)), ref_elem);
            }
            else if (operand->type == ZIR_OPERAND_SYMBOL)
            {
                struct ZirSymbolOperand *symbol_elem = (struct ZirSymbolOperand*)operand;
                zenit_nes_emit_store_symbol(program, nes_symbol, offset + (i * zir_type_size(symbol_elem->symbol->typeinfo)), symbol_elem);
            }
        }
    }
}

void zenit_nes_emit_store_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, size_t offset, struct ZirSymbolOperand *symbol_operand)
{
    uint16_t target_address = nes_symbol->address + offset;
    struct ZenitNesSymbol *source_symbol = fl_hashtable_get(program->symbols, symbol_operand->symbol->name);

    // We need to get the destination slot
    uint8_t *destination = NULL;
    // FIXME: Handle ZP and CODE
    if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
    {
        destination = program->data.bytes + (target_address - program->data.base_address);
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_TMP)
    {
        destination = program->tmp.bytes + target_address;
    }

    // Now we need to know from where we need to get the source values
    uint8_t *source = NULL;
    // FIXME: Handle ZP and CODE
    if (source_symbol->segment == ZENIT_NES_SEGMENT_TMP)
    {
        source = program->tmp.bytes + source_symbol->address;
    }
    else if (source_symbol->segment == ZENIT_NES_SEGMENT_DATA)
    {
        source = program->data.bytes + source_symbol->address - program->data.base_address;
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

    if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP || nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
    {
        zenit_nes_program_emit_imm(&program->startup, NES_OP_LDA, (uint8_t)(ref_op_symbol->address & 0xFF));
        zenit_nes_program_emit_imm(&program->startup, NES_OP_LDX, (uint8_t)((ref_op_symbol->address >> 8) & 0xFF));
        zenit_nes_program_emit_zpg(&program->startup, NES_OP_STA, (uint8_t)(target_address));
        zenit_nes_program_emit_zpg(&program->startup, NES_OP_STX, (uint8_t)(target_address + 1));
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
        if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP || nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
        {
            zenit_nes_program_emit_imm(&program->startup, NES_OP_LDA, uint_operand->value.uint8);
            zenit_nes_program_emit_zpg(&program->startup, NES_OP_STA, (uint8_t)(target_address));

            // upcast from uint8 to uint16
            if (nes_symbol->element_size > sizeof(uint8_t))
            {
                zenit_nes_program_emit_imm(&program->startup, NES_OP_LDX, 0x0);
                zenit_nes_program_emit_zpg(&program->startup, NES_OP_STX, (uint8_t)(target_address + 1));
            }
        }
        else if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
        {
            uint8_t *slot = program->data.bytes + (target_address - program->data.base_address);
            *slot = uint_operand->value.uint8;
            
            // upcast from uint8 to uint16
            if (nes_symbol->element_size > sizeof(uint8_t))
            {
                slot++;
                size_t to_fill = sizeof(uint8_t) * (nes_symbol->element_size - 1);
                for (size_t i=0; i < to_fill; i++)
                    *(slot + i) = 0;
            }
        }
        else if (nes_symbol->segment == ZENIT_NES_SEGMENT_TMP)
        {
            uint8_t *slot = program->tmp.bytes + target_address;
            *slot = uint_operand->value.uint8;
            
            // upcast from uint8 to uint16
            if (nes_symbol->element_size > sizeof(uint8_t))
            {
                slot++;
                size_t to_fill = sizeof(uint8_t) * (nes_symbol->element_size - 1);
                for (size_t i=0; i < to_fill; i++)
                    *(slot + i) = 0;
            }
        }
    }
    else if (uint_operand->typeinfo->size == ZIR_UINT_16)
    {
        if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP || nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
        {
            zenit_nes_program_emit_imm(&program->startup, NES_OP_LDA, (uint8_t)(uint_operand->value.uint16 & 0xFF));
            zenit_nes_program_emit_imm(&program->startup, NES_OP_LDX, (uint8_t)((uint_operand->value.uint16 >> 8) & 0xFF));
            zenit_nes_program_emit_zpg(&program->startup, NES_OP_STA, (uint8_t)(target_address));
            zenit_nes_program_emit_zpg(&program->startup, NES_OP_STX, (uint8_t)(target_address + 1));
        }
        else if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
        {
            uint8_t *slot = program->data.bytes + (target_address - program->data.base_address);
            *slot       = uint_operand->value.uint16 & 0xFF;
            *(slot+1)   = (uint_operand->value.uint16 >> 8) & 0xFF;
        }
        else if (nes_symbol->segment == ZENIT_NES_SEGMENT_TMP)
        {
            uint8_t *slot = program->tmp.bytes + target_address;
            *slot       = uint_operand->value.uint16 & 0xFF;
            *(slot+1)   = (uint_operand->value.uint16 >> 8) & 0xFF;
        }
    }
}
