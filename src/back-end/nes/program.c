#include "program.h"
#include "opcode.h"

#include "../../zir/symtable.h"
#include "../../zir/block.h"
#include "../../zir/instructions/attributes/attribute.h"
#include "../../zir/instructions/operands/uint.h"

void zenit_nes_symbol_free(struct ZenitNesSymbol *symbol)
{
    if (symbol->name)
        fl_cstring_free(symbol->name);

    fl_free(symbol);
}

struct ZenitNesProgram* zenit_nes_program_new()
{
    struct ZenitNesProgram *program = fl_malloc(sizeof(struct ZenitNesProgram));

    // We start in the static context (global)
    program->static_context = true;

    program->code = (struct ZenitNesCodeSegment) {
        .pc = 0,
        .bytes = fl_array_new(sizeof(uint8_t), UINT16_MAX),
    };

    program->symbols = fl_hashtable_new_args((struct FlHashtableArgs) {
        .hash_function = fl_hashtable_hash_string,
        .key_allocator = fl_container_allocator_string,
        .key_comparer = fl_container_equals_string,
        .key_cleaner = fl_container_cleaner_pointer,
        .value_cleaner = (FlContainerCleanupFunction)zenit_nes_symbol_free,
        .value_allocator = NULL
    });

    program->zp = (struct ZenitNesZeroPageSegment){
        .slots = { 0 },
    };

    program->startup = (struct ZenitNesCodeSegment) {
        .pc = 0,
        .bytes = fl_array_new(sizeof(uint8_t), UINT16_MAX),
    };

    program->data = (struct ZenitNesDataSegment){
        .base_address = 0x8000,
        .slots = fl_array_new(sizeof(uint8_t), 0x8000),
        .bytes = fl_array_new(sizeof(uint8_t), 0x8000),
    };

    return program;
}

void zenit_nes_program_free(struct ZenitNesProgram *program)
{
    if (!program)
        return;

    fl_array_free(program->code.bytes);

    fl_hashtable_free(program->symbols);

    fl_array_free(program->startup.bytes);

    fl_array_free(program->data.slots);
    fl_array_free(program->data.bytes);

    fl_free(program);
}

static inline bool reserve_zp_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol **nes_symbol, struct ZirSymbol *zir_symbol, uint8_t *address)
{
    if (!program || !nes_symbol || !zir_symbol)
        return false;

    size_t max_size = 0xFF;

    // We need to get the symbol size to make sure it fits
    size_t symbol_size = zir_type_size(zir_symbol->typeinfo);
    
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
    (*nes_symbol)->elements = zir_symbol->typeinfo->type == ZIR_TYPE_ARRAY ? ((struct ZirArrayTypeInfo*) zir_symbol->typeinfo)->length : 1;
    (*nes_symbol)->size = symbol_size;
    (*nes_symbol)->element_size = symbol_size / (*nes_symbol)->elements;
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
    size_t symbol_size = zir_symbol->typeinfo->type == ZIR_TYPE_REFERENCE ? 2 /*bytes*/ : zir_type_size(zir_symbol->typeinfo);

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
    (*nes_symbol)->elements = zir_symbol->typeinfo->type == ZIR_TYPE_ARRAY ? ((struct ZirArrayTypeInfo*) zir_symbol->typeinfo)->length : 1;
    (*nes_symbol)->size = symbol_size;
    (*nes_symbol)->element_size = symbol_size / (*nes_symbol)->elements;
    (*nes_symbol)->address = program->data.base_address + (uint16_t)slot;

    fl_hashtable_add(program->symbols, (*nes_symbol)->name, *nes_symbol);

    return true;
}

static inline bool reserve_temp_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol **nes_symbol, struct ZirSymbol *zir_symbol)
{
    if (!program || !nes_symbol || !zir_symbol)
        return false;

    // We need to get the symbol size to make sure it fits
    size_t symbol_size = zir_symbol->typeinfo->type == ZIR_TYPE_REFERENCE ? 2 /*bytes*/ : zir_type_size(zir_symbol->typeinfo);

    struct ZenitNesTmpSymbol *temp_symbol = fl_malloc(sizeof(struct ZenitNesTmpSymbol));

    temp_symbol->base.name = fl_cstring_dup(zir_symbol->name);
    temp_symbol->base.segment = ZENIT_NES_SEGMENT_TEMP;
    temp_symbol->base.elements = zir_symbol->typeinfo->type == ZIR_TYPE_ARRAY ? ((struct ZirArrayTypeInfo*) zir_symbol->typeinfo)->length : 1;
    temp_symbol->base.size = symbol_size;
    temp_symbol->base.element_size = symbol_size / temp_symbol->base.elements;
    temp_symbol->base.address = 0; // Mind that it being a temp symbol means we don't actually use the address

    *nes_symbol = (struct ZenitNesSymbol*)temp_symbol;
    fl_hashtable_add(program->symbols, temp_symbol->base.name, *nes_symbol);

    return true;
}

static inline bool map_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol **nes_symbol, struct ZirSymbol *zir_symbol, uint16_t address)
{
    *nes_symbol = fl_malloc(sizeof(struct ZenitNesSymbol));

    (*nes_symbol)->name = fl_cstring_dup(zir_symbol->name);
    (*nes_symbol)->segment = ZENIT_NES_SEGMENT_CODE;
    (*nes_symbol)->elements = zir_symbol->typeinfo->type == ZIR_TYPE_ARRAY ? ((struct ZirArrayTypeInfo*) zir_symbol->typeinfo)->length : 1;
    (*nes_symbol)->size = zir_type_size(zir_symbol->typeinfo);
    (*nes_symbol)->element_size = (*nes_symbol)->size / (*nes_symbol)->elements;
    (*nes_symbol)->address = address;

    fl_hashtable_add(program->symbols, (*nes_symbol)->name, *nes_symbol);

    return true;
}


struct ZenitNesSymbol* zenit_nes_program_reserve_symbol(struct ZenitNesProgram *program, struct ZirBlock *block, struct ZirAttributeMap *attributes, struct ZirSymbol *zir_symbol)
{
    struct ZenitNesSymbol *nes_symbol = NULL;

    if (zir_symbol->name[0] == '%')
    {
        reserve_temp_symbol(program, &nes_symbol, zir_symbol);
        return nes_symbol;
    }
    
    bool is_global = block->symtable.type == ZIR_SYMTABLE_GLOBAL;

    if (attributes != NULL && zir_attribute_map_has_key(attributes, "NES"))
    {
        struct ZirAttribute *nes_attribute = zir_attribute_map_get(attributes, "NES");

        if (zir_property_map_has_key(&nes_attribute->properties, "segment"))
        {
            struct ZirProperty *segment_property = zir_property_map_get(&nes_attribute->properties, "segment");

            if (segment_property->value->type == ZIR_OPERAND_SYMBOL)
            {
                struct ZirSymbolOperand *symbol_operand = (struct ZirSymbolOperand*)segment_property->value;

                if (flm_cstring_equals(symbol_operand->symbol->name, "zp"))
                    reserve_zp_symbol(program, &nes_symbol, zir_symbol, NULL);
                else if (flm_cstring_equals(symbol_operand->symbol->name, "data"))
                    reserve_data_symbol(program, &nes_symbol, zir_symbol, NULL);
            }
        }
        else if (zir_property_map_has_key(&nes_attribute->properties, "address"))
        {
            struct ZirProperty *address_property = zir_property_map_get(&nes_attribute->properties, "address");
            struct ZirUintOperand *uint_value = (struct ZirUintOperand*)address_property->value;

            if (uint_value->typeinfo->size == ZIR_UINT_8)
            {
                reserve_zp_symbol(program, &nes_symbol, zir_symbol, &uint_value->value.uint8);
            }
            else if (uint_value->typeinfo->size == ZIR_UINT_16)
            {
                if (uint_value->value.uint16 >= program->data.base_address)
                {
                    reserve_data_symbol(program, &nes_symbol, zir_symbol, &uint_value->value.uint16);
                }
                else
                {
                    map_symbol(program, &nes_symbol, zir_symbol, uint_value->value.uint16);
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

/*
uint16_t zenit_nes_program_emit_label(struct ZenitNesCodeSegment *code)
{
    return code->pc + program->base_address;
}

uint8_t zenit_nes_program_calc_rel_addr(struct ZenitNesCodeSegment *code, uint16_t address)
{
    if (address <= code->pc + program->base_address)
        return 256 - (code->pc + program->base_address + 2 - address); // 2 is for the 2-byte instruction

    return address - code->pc + program->base_address;
}
*/
void zenit_nes_program_emit_abs(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint16_t bytes)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_ABS);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = (uint8_t)(bytes);
    code->bytes[code->pc++] = (uint8_t)(bytes >> 8);
}

void zenit_nes_program_emit_abx(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint16_t bytes)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_ABX);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = (uint8_t)(bytes);
    code->bytes[code->pc++] = (uint8_t)(bytes >> 8);
}

void zenit_nes_program_emit_aby(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint16_t bytes)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_ABY);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = (uint8_t)(bytes);
    code->bytes[code->pc++] = (uint8_t)(bytes >> 8);
}

void zenit_nes_program_emit_imm(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint8_t byte)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_IMM);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = byte;
}

void zenit_nes_program_emit_imp(struct ZenitNesCodeSegment *code, enum NesOpcode opcode)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_IMP);
    code->bytes[code->pc++] = opcode_hex;
}

void zenit_nes_program_emit_ind(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint16_t bytes)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_IND);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = (uint8_t)(bytes);
    code->bytes[code->pc++] = (uint8_t)(bytes >> 8);
}

void zenit_nes_program_emit_inx(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint8_t byte)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_INX);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = byte;
}

void zenit_nes_program_emit_iny(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint8_t byte)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_INY);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = byte;
}

void zenit_nes_program_emit_rel(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint8_t byte)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_REL);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = byte;
}

void zenit_nes_program_emit_zpg(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint8_t byte)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_ZPG);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = byte;
}

void zenit_nes_program_emit_zpx(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint8_t byte)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_ZPX);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = byte;
}

void zenit_nes_program_emit_zpy(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint8_t byte)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_ZPY);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = byte;
}
