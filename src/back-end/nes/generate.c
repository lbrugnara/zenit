#include <stdint.h>
#include <fllib.h>
#include "generate.h"
#include "program.h"

#include "../../zir/symtable.h"

static inline bool reserve_zp_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol **nes_symbol, struct ZenitIrSymbol *zir_symbol, uint8_t *address)
{
    if (!program || !nes_symbol || !zir_symbol)
        return false;

    size_t max_size = 0xFF;

    // We need to get the symbol size to make sure it fits
    size_t symbol_size = zenit_ir_type_size(&zir_symbol->typeinfo);
    
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
    (*nes_symbol)->size = symbol_size;
    (*nes_symbol)->address = slot;

    fl_hashtable_add(program->symbols, (*nes_symbol)->name, *nes_symbol);

    return true;
}

static inline bool reserve_data_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol **nes_symbol, struct ZenitIrSymbol *zir_symbol, uint16_t *address)
{
    if (!program || !nes_symbol || !zir_symbol)
        return false;

    size_t max_size = fl_array_length(program->data.slots);

    // We need to get the symbol size to make sure it fits
    size_t symbol_size = zenit_ir_type_size(&zir_symbol->typeinfo);

    size_t slot = 0;

    if (address)
    {
        slot = *address;

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
    (*nes_symbol)->size = symbol_size;
    (*nes_symbol)->address = program->data.base_address + (uint16_t)slot;

    fl_hashtable_add(program->symbols, (*nes_symbol)->name, *nes_symbol);

    return true;
}

static inline bool map_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol **nes_symbol, struct ZenitIrSymbol *zir_symbol, uint16_t address)
{
    *nes_symbol = fl_malloc(sizeof(struct ZenitNesSymbol));

    (*nes_symbol)->name = fl_cstring_dup(zir_symbol->name);
    (*nes_symbol)->segment = ZENIT_NES_SEGMENT_CODE;
    (*nes_symbol)->size = zenit_ir_type_size(&zir_symbol->typeinfo);
    (*nes_symbol)->address = address;

    fl_hashtable_add(program->symbols, (*nes_symbol)->name, *nes_symbol);

    return true;
}

typedef void(*ZenitIrBlockVisitor)(struct ZenitIrBlock *block, struct ZenitNesProgram *program);
typedef void(*ZenitIrInstructionVisitor)(struct ZenitIrInstruction *instruction, struct ZenitIrBlock *block, struct ZenitNesProgram *program);

static void visit_block(struct ZenitIrBlock *block, struct ZenitNesProgram *program);
static void visit_instruction(struct ZenitIrInstruction *instruction, struct ZenitIrBlock *block, struct ZenitNesProgram *program);
static void visit_variable_instruction(struct ZenitIrVariableInstruction *instruction, struct ZenitIrBlock *block, struct ZenitNesProgram *program);

static const ZenitIrInstructionVisitor instruction_visitors[] = {
    // These nodes introduce new symbols into the symbol table or retrieve symbols from it
    [ZENIT_IR_INSTR_VARIABLE]   = (ZenitIrInstructionVisitor)&visit_variable_instruction
    
};

static void visit_variable_instruction(struct ZenitIrVariableInstruction *instruction, struct ZenitIrBlock *block, struct ZenitNesProgram *program)
{
    struct ZenitIrSymbol *zir_symbol = instruction->lvalue.operand.symbol;
    struct ZenitNesSymbol *nes_symbol = NULL;

    bool is_global = block->symtable.type == ZENIT_IR_SYMTABLE_GLOBAL;
    bool is_zp = false;
    bool is_data = false;

    if (zenit_ir_attribute_map_has_key(&instruction->attributes, "NES"))
    {
        struct ZenitIrAttribute *nes_attribute = zenit_ir_attribute_map_get(&instruction->attributes, "NES");

        if (zenit_ir_property_map_has_key(&nes_attribute->properties, "segment"))
        {
            struct ZenitIrProperty *segment_property = zenit_ir_property_map_get(&nes_attribute->properties, "segment");

            if (segment_property->value.type == ZENIT_IR_OPERAND_SYMBOL)
            {
                if (flm_cstring_equals(segment_property->value.operand.symbol->name, "zp"))
                {
                    reserve_zp_symbol(program, &nes_symbol, zir_symbol, NULL);
                    is_zp = true;
                }
                else if (flm_cstring_equals(segment_property->value.operand.symbol->name, "data"))
                {
                    reserve_data_symbol(program, &nes_symbol, zir_symbol, NULL);
                    is_data = true;
                }
            }
        }
        else if (zenit_ir_property_map_has_key(&nes_attribute->properties, "address"))
        {
            struct ZenitIrProperty *address_property = zenit_ir_property_map_get(&nes_attribute->properties, "address");
            struct ZenitIrLiteralValue *literal_value = (struct ZenitIrLiteralValue*)address_property->value.operand.value;

            if (literal_value->base.typeinfo.type == ZENIT_IR_TYPE_UINT8)
            {
                reserve_zp_symbol(program, &nes_symbol, zir_symbol, &literal_value->value.uint8);
                is_zp = true;
            }
            else if (literal_value->base.typeinfo.type == ZENIT_IR_TYPE_UINT16)
            {
                if (literal_value->value.uint16 >= program->data.base_address)
                {
                    reserve_data_symbol(program, &nes_symbol, zir_symbol, &literal_value->value.uint16);
                    is_data = true;
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
            is_data = true;
        }
        else
        {
            // FIXME: Add the symbol to the stack
        }
    }

    if (instruction->rvalue.type == ZENIT_IR_OPERAND_VALUE)
    {
        struct ZenitIrLiteralValue *literal = (struct ZenitIrLiteralValue*)instruction->rvalue.operand.value;

        if (literal->base.typeinfo.type == ZENIT_IR_TYPE_UINT8)
        {
            if (is_zp || !is_data)
            {
                zenit_nes_program_emit_imm(&program->init, NES_OP_LDA, literal->value.uint8);
                zenit_nes_program_emit_imm(&program->init, NES_OP_STA, (uint8_t)(nes_symbol->address & 0xFF));
            }
            else if (is_data)
            {
                uint8_t *slot = program->data.bytes + (nes_symbol->address - program->data.base_address);
                memcpy(slot, &literal->value.uint8, sizeof(uint8_t));
            }
        }
        else if (literal->base.typeinfo.type == ZENIT_IR_TYPE_UINT16)
        {
            if (is_zp || !is_data)
            {
                zenit_nes_program_emit_imm(&program->init, NES_OP_LDX, (uint8_t)(literal->value.uint16 >> 8));
                zenit_nes_program_emit_imm(&program->init, NES_OP_LDA, (uint8_t)(literal->value.uint16 & 0xFF));
                zenit_nes_program_emit_imm(&program->init, NES_OP_STA, (uint8_t)(nes_symbol->address & 0xFF));
                zenit_nes_program_emit_imm(&program->init, NES_OP_STX, (uint8_t)(nes_symbol->address + 0x1 & 0xFF));
            }
            else if (is_data)
            {
                uint8_t *slot = program->data.bytes + (nes_symbol->address - program->data.base_address);
                memcpy(slot, &literal->value.uint16, sizeof(uint16_t));
            }
        }
    }
    else
    {
        // FIXME: Handle symbols and refs
    }
}

static void visit_instruction(struct ZenitIrInstruction *instruction, struct ZenitIrBlock *block, struct ZenitNesProgram *program)
{
    instruction_visitors[instruction->type](instruction, block, program);
}

static void visit_block(struct ZenitIrBlock *block, struct ZenitNesProgram *program)
{
    size_t instr_count = fl_array_length(block->instructions);

    for (size_t i=0; i < instr_count; i++)
    {
        struct ZenitIrInstruction *instr = block->instructions[i];
        visit_instruction(instr, block, program);
    }
}

struct ZenitNesProgram* zenit_nes_generate(struct ZenitIrProgram *zir_program)
{
    if (!zir_program)
        return NULL;

    // Create the program object
    struct ZenitNesProgram *nes_program = zenit_nes_program_new();

    // Start visiting the ZIR program
    struct ZenitIrBlock *zir_block = zir_program->current;
    visit_block(zir_block, nes_program);
    
    return nes_program;
}
