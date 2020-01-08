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

    // FIXME: Handle ZP and CODE
    if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
    {
        uint8_t *slot = program->data.bytes + (target_address - program->data.base_address);

        // FIXME: Handle casts (fill)
        if (source_symbol->segment == ZENIT_NES_SEGMENT_TMP)
        {
            for (size_t i=0; i < source_symbol->size; i++)
            {
                *(slot+i) = program->tmp.bytes[source_symbol->address + i];
            }
        }
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
