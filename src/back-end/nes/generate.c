#include <stdint.h>
#include <fllib.h>
#include "generate.h"
#include "program.h"

#include "../../zir/symtable.h"

typedef void(*ZirBlockVisitor)(struct ZirBlock *block, struct ZenitNesProgram *program);
typedef void(*ZirInstructionVisitor)(struct ZirInstruction *instruction, struct ZirBlock *block, struct ZenitNesProgram *program);

static void visit_block(struct ZirBlock *block, struct ZenitNesProgram *program);
static void visit_instruction(struct ZirInstruction *instruction, struct ZirBlock *block, struct ZenitNesProgram *program);
static void visit_variable_instruction(struct ZirVariableInstruction *instruction, struct ZirBlock *block, struct ZenitNesProgram *program);
static void visit_cast_instruction(struct ZirCastInstruction *instruction, struct ZirBlock *block, struct ZenitNesProgram *program);

static const ZirInstructionVisitor instruction_visitors[] = {
    [ZIR_INSTR_VARIABLE]   = (ZirInstructionVisitor)&visit_variable_instruction,
    [ZIR_INSTR_CAST]       = (ZirInstructionVisitor)&visit_cast_instruction,
};

static void emit_array_store(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, struct ZirArrayValue *array, size_t offset);
static void emit_literal_store(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, struct ZirLiteralValue *literal, size_t offset);

static inline bool reserve_zp_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol **nes_symbol, struct ZirSymbol *zir_symbol, uint8_t *address)
{
    if (!program || !nes_symbol || !zir_symbol)
        return false;

    size_t max_size = 0xFF;

    // We need to get the symbol size to make sure it fits
    size_t symbol_size = zir_type_size(&zir_symbol->typeinfo);
    
    size_t slot = 0;

    if (address)
    {
        slot = *address;

        if (slot + symbol_size > max_size)
            return false;

        for (size_t i=slot; i < slot + symbol_size; i++)
        {
            // If used, it doesn't fit, so return
            if (program->zp.slots[i] != 0)
                return false;
        }

        // Flag it as used
        memset(program->zp.slots + slot, 1, symbol_size);
    }
    else
    {        
        for (size_t i=0; i < max_size; i++)
        {
            // Doesn't fit
            if (i + symbol_size >= max_size)
                return false;

            if (program->zp.slots[i] != 0)
                continue;

            bool fit = true;
            
            for (size_t j=i; j < i + symbol_size; j++)
            {
                // If used, it doesn't fit, so return
                if (program->zp.slots[j] != 0)
                {
                    fit = false;
                    break;
                }
            }

            if (!fit)
                continue;

            slot = i;

            // Flag it as used
            memset(program->zp.slots + slot, 1, symbol_size);
            break;
        }
    }

    *nes_symbol = fl_malloc(sizeof(struct ZenitNesSymbol));

    (*nes_symbol)->name = fl_cstring_dup(zir_symbol->name);
    (*nes_symbol)->segment = ZENIT_NES_SEGMENT_ZP;
    (*nes_symbol)->elements = zir_symbol->typeinfo.elements;
    (*nes_symbol)->size = symbol_size;
    (*nes_symbol)->element_size = symbol_size / zir_symbol->typeinfo.elements;
    (*nes_symbol)->address = slot;

    fl_hashtable_add(program->symbols, (*nes_symbol)->name, *nes_symbol);

    return true;
}

static inline bool reserve_data_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol **nes_symbol, struct ZirSymbol *zir_symbol, uint16_t *address)
{
    if (!program || !nes_symbol || !zir_symbol)
        return false;

    size_t max_size = fl_array_length(program->data.slots);

    // We need to get the symbol size to make sure it fits
    size_t symbol_size = zir_type_size(&zir_symbol->typeinfo);

    size_t slot = 0;

    if (address)
    {
        slot = *address - program->data.base_address;

        if (slot + symbol_size > max_size)
            return false;

        for (size_t i=slot; i < slot + symbol_size; i++)
        {
            // If used, it doesn't fit, so return
            if (program->data.slots[i] != 0)
                return false;
        }

        // Flag it as used
        memset(program->data.slots + slot, 1, symbol_size);
    }
    else
    {
        for (size_t i=0; i < max_size; i++)
        {
            // Doesn't fit
            if (i + symbol_size >= max_size)
                return false;

            if (program->data.slots[i] != 0)
                continue;

            bool fit = true;
            
            for (size_t j=i; j < i + symbol_size; j++)
            {
                // If used, it doesn't fit, so return
                if (program->data.slots[j] != 0)
                {
                    fit = false;
                    break;
                }
            }

            if (!fit)
                continue;

            slot = i;

            // Flag it as used
            memset(program->data.slots + slot, 1, symbol_size);
            break;
        }
    }

    *nes_symbol = fl_malloc(sizeof(struct ZenitNesSymbol));

    (*nes_symbol)->name = fl_cstring_dup(zir_symbol->name);
    (*nes_symbol)->segment = ZENIT_NES_SEGMENT_DATA;
    (*nes_symbol)->elements = zir_symbol->typeinfo.elements;
    (*nes_symbol)->size = symbol_size;
    (*nes_symbol)->element_size = symbol_size / zir_symbol->typeinfo.elements;
    (*nes_symbol)->address = program->data.base_address + (uint16_t)slot;

    fl_hashtable_add(program->symbols, (*nes_symbol)->name, *nes_symbol);

    return true;
}

static inline bool map_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol **nes_symbol, struct ZirSymbol *zir_symbol, uint16_t address)
{
    *nes_symbol = fl_malloc(sizeof(struct ZenitNesSymbol));

    (*nes_symbol)->name = fl_cstring_dup(zir_symbol->name);
    (*nes_symbol)->segment = ZENIT_NES_SEGMENT_CODE;
    (*nes_symbol)->elements = zir_symbol->typeinfo.elements;
    (*nes_symbol)->size = zir_type_size(&zir_symbol->typeinfo);
    (*nes_symbol)->element_size = (*nes_symbol)->size / zir_symbol->typeinfo.elements;
    (*nes_symbol)->address = address;

    fl_hashtable_add(program->symbols, (*nes_symbol)->name, *nes_symbol);

    return true;
}


static struct ZenitNesSymbol* reserve_symbol(struct ZenitNesProgram *program, struct ZirBlock *block, struct ZirVariableInstruction *instruction, struct ZirSymbol *zir_symbol)
{
    bool is_global = block->symtable.type == ZIR_SYMTABLE_GLOBAL;

    struct ZenitNesSymbol *nes_symbol = NULL;

    if (zir_attribute_map_has_key(&instruction->attributes, "NES"))
    {
        struct ZirAttribute *nes_attribute = zir_attribute_map_get(&instruction->attributes, "NES");

        if (zir_property_map_has_key(&nes_attribute->properties, "segment"))
        {
            struct ZirProperty *segment_property = zir_property_map_get(&nes_attribute->properties, "segment");

            if (segment_property->value.type == ZIR_OPERAND_SYMBOL)
            {
                if (flm_cstring_equals(segment_property->value.operand.symbol->name, "zp"))
                {
                    reserve_zp_symbol(program, &nes_symbol, zir_symbol, NULL);
                }
                else if (flm_cstring_equals(segment_property->value.operand.symbol->name, "data"))
                {
                    reserve_data_symbol(program, &nes_symbol, zir_symbol, NULL);
                }
            }
        }
        else if (zir_property_map_has_key(&nes_attribute->properties, "address"))
        {
            struct ZirProperty *address_property = zir_property_map_get(&nes_attribute->properties, "address");
            struct ZirLiteralValue *literal_value = (struct ZirLiteralValue*)address_property->value.operand.value;

            if (literal_value->base.typeinfo.type == ZIR_TYPE_UINT8)
            {
                reserve_zp_symbol(program, &nes_symbol, zir_symbol, &literal_value->value.uint8);
            }
            else if (literal_value->base.typeinfo.type == ZIR_TYPE_UINT16)
            {
                if (literal_value->value.uint16 >= program->data.base_address)
                {
                    reserve_data_symbol(program, &nes_symbol, zir_symbol, &literal_value->value.uint16);
                }
                else
                {
                    map_symbol(program, &nes_symbol, zir_symbol, literal_value->value.uint16);
                }
            }
            
        }
    }

    if (nes_symbol == NULL)
    {
        if (is_global)
        {
            reserve_data_symbol(program, &nes_symbol, zir_symbol, NULL);
        }
        else
        {
            // FIXME: Add the symbol to the stack
        }
    }

    return nes_symbol;
}

static void emit_array_store(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, struct ZirArrayValue *array, size_t offset)
{
    uint16_t symbol_address = nes_symbol->address + offset;

    if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
    {
        // If the symbol is in the data segment, we just assign the value
        for (size_t i=0; i < fl_array_length(array->elements); i++)
        {
            struct ZirOperand element = array->elements[i];
            uint8_t *slot = program->data.bytes + (nes_symbol->address - program->data.base_address) + i * nes_symbol->size;
            
            if (element.type == ZIR_OPERAND_VALUE)
            {
                if (element.operand.value->type == ZIR_VALUE_LITERAL)
                {
                    struct ZirLiteralValue *literal_elem = (struct ZirLiteralValue*)element.operand.value;
                    emit_literal_store(program, nes_symbol, literal_elem, offset + (i * zir_type_size(&literal_elem->base.typeinfo)));
                }
                else
                {
                    struct ZirArrayValue *array_elem = (struct ZirArrayValue*)element.operand.value;
                    emit_array_store(program, nes_symbol, array_elem, offset + (i * zir_type_size(&array_elem->base.typeinfo)));
                }
            }
            else
            {
                // FIXME: Handle refs and symbols
            }
        }
    }
}

static void emit_literal_store(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, struct ZirLiteralValue *literal, size_t offset)
{
    uint16_t symbol_address = nes_symbol->address + offset;

    if (literal->base.typeinfo.type == ZIR_TYPE_UINT8)
    {
        if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP || nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
        {
            zenit_nes_program_emit_imm(&program->startup, NES_OP_LDA, literal->value.uint8);
            zenit_nes_program_emit_zpg(&program->startup, NES_OP_STA, (uint8_t)(symbol_address));

            // upcast from uint8 to uint16
            if (nes_symbol->element_size > sizeof(uint8_t))
            {
                zenit_nes_program_emit_imm(&program->startup, NES_OP_LDX, 0x0);
                zenit_nes_program_emit_zpg(&program->startup, NES_OP_STX, (uint8_t)(symbol_address + 1));
            }
        }
        else if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
        {
            uint8_t *slot = program->data.bytes + (symbol_address - program->data.base_address);
            *slot = literal->value.uint8;
            
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
    else if (literal->base.typeinfo.type == ZIR_TYPE_UINT16)
    {
        if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP || nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
        {
            zenit_nes_program_emit_imm(&program->startup, NES_OP_LDA, (uint8_t)(literal->value.uint16 & 0xFF));
            zenit_nes_program_emit_imm(&program->startup, NES_OP_LDX, (uint8_t)((literal->value.uint16 >> 8) & 0xFF));
            zenit_nes_program_emit_zpg(&program->startup, NES_OP_STA, (uint8_t)(symbol_address));
            zenit_nes_program_emit_zpg(&program->startup, NES_OP_STX, (uint8_t)(symbol_address + 1));
        }
        else if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
        {
            uint8_t *slot = program->data.bytes + (symbol_address - program->data.base_address);
            *slot       = literal->value.uint16 & 0xFF;
            *(slot+1)   = (literal->value.uint16 >> 8) & 0xFF;
        }
    }
}

static void visit_cast_instruction(struct ZirCastInstruction *instruction, struct ZirBlock *block, struct ZenitNesProgram *program)
{
    //struct ZirSymbol *zir_symbol = instruction->lvalue.operand.symbol;
    //struct ZenitNesSymbol *nes_symbol = reserve_symbol(program, block, instruction, zir_symbol);
}

static void visit_variable_instruction(struct ZirVariableInstruction *instruction, struct ZirBlock *block, struct ZenitNesProgram *program)
{
    struct ZirSymbol *zir_symbol = instruction->lvalue.operand.symbol;
    struct ZenitNesSymbol *nes_symbol = reserve_symbol(program, block, instruction, zir_symbol);

    if (instruction->rvalue.type == ZIR_OPERAND_VALUE)
    {
        if (instruction->rvalue.operand.value->type == ZIR_VALUE_LITERAL)
        {
            emit_literal_store(program, nes_symbol, (struct ZirLiteralValue*)instruction->rvalue.operand.value, 0);
        }
        else
        {
            emit_array_store(program, nes_symbol, (struct ZirArrayValue*)instruction->rvalue.operand.value, 0);
        }
    }
    else if (instruction->rvalue.type == ZIR_OPERAND_REFERENCE)
    {
        // FIXME: Handle symbols and refs
    }
    else if (instruction->rvalue.type == ZIR_OPERAND_SYMBOL)
    {
        // FIXME: Handle symbols
    }
}

static void visit_instruction(struct ZirInstruction *instruction, struct ZirBlock *block, struct ZenitNesProgram *program)
{
    instruction_visitors[instruction->type](instruction, block, program);
}

static void visit_block(struct ZirBlock *block, struct ZenitNesProgram *program)
{
    size_t instr_count = fl_array_length(block->instructions);

    for (size_t i=0; i < instr_count; i++)
    {
        struct ZirInstruction *instr = block->instructions[i];
        visit_instruction(instr, block, program);
    }
}

struct ZenitNesProgram* zenit_nes_generate_program(struct ZirProgram *zir_program)
{
    if (!zir_program)
        return NULL;

    // Create the program object
    struct ZenitNesProgram *nes_program = zenit_nes_program_new();

    // Start visiting the ZIR program
    struct ZirBlock *zir_block = zir_program->current;
    visit_block(zir_block, nes_program);


    
    return nes_program;
}
