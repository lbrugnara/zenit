#include <fllib/Cstring.h>
#include "program.h"
#include "opcode.h"

#include "symbols/array.h"
#include "symbols/reference.h"
#include "symbols/struct.h"
#include "symbols/temp.h"
#include "symbols/uint.h"

#include "../../zir/symtable.h"
#include "../../zir/block.h"
#include "../../zir/instructions/attributes/attribute.h"
#include "../../zir/instructions/operands/uint.h"

void zenit_nes_symbol_free(struct ZenitNesSymbol *symbol)
{
    if (symbol->name)
        fl_cstring_free(symbol->name);

    if (symbol->symkind == ZENIT_NES_SYMBOL_ARRAY)
    {
        struct ZenitNesArraySymbol *array_symbol = (struct ZenitNesArraySymbol*) symbol;
        fl_array_free_each_pointer(array_symbol->elements, (FlArrayFreeElementFunc) zenit_nes_symbol_free);
    }
    else if (symbol->symkind == ZENIT_NES_SYMBOL_STRUCT)
    {
        struct ZenitNesStructSymbol *struct_symbol = (struct ZenitNesStructSymbol*) symbol;
        fl_array_free_each_pointer(struct_symbol->members, (FlArrayFreeElementFunc) zenit_nes_symbol_free);
    }

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
        .value_cleaner = (FlContainerCleanupFunction) zenit_nes_symbol_free,
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

struct ZenitNesSymbol* zenit_nes_program_get_tmpsym_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol *temp_symbol)
{
    if (temp_symbol->segment != ZENIT_NES_SEGMENT_TEMP)
        return NULL;

    struct ZenitNesSymbol *target_symbol = NULL;

    struct ZirOperand *temp_operand = ((struct ZenitNesTempSymbol*) temp_symbol)->source;

    while (temp_operand)
    {
        if (temp_operand->type != ZIR_OPERAND_SYMBOL)
            break;

        struct ZenitNesSymbol *tmp_symbol = fl_hashtable_get(program->symbols, ((struct ZirSymbolOperand*) temp_operand)->symbol->name);

        if (tmp_symbol == NULL)
            break;

        if (tmp_symbol->segment == ZENIT_NES_SEGMENT_TEMP)
        {
            temp_operand = ((struct ZenitNesTempSymbol*) tmp_symbol)->source;
            continue;
        }

        struct ZenitNesSymbol *symbol = fl_hashtable_get(program->symbols, ((struct ZirSymbolOperand*) temp_operand)->symbol->name);
            
        target_symbol = symbol;
        break;
    }

    return target_symbol;
}

struct ZirOperand* zenit_nes_program_get_tmpsym_operand(struct ZenitNesProgram *program, struct ZenitNesSymbol *temp_symbol, enum ZirOperandType type)
{
    if (temp_symbol->segment != ZENIT_NES_SEGMENT_TEMP)
        return NULL;

    struct ZirOperand *temp_operand = ((struct ZenitNesTempSymbol*) temp_symbol)->source;

    while (temp_operand)
    {
        if (temp_operand->type == type)
            return temp_operand;

        if (temp_operand->type != ZIR_OPERAND_SYMBOL)
            break;

        struct ZenitNesSymbol *tmp_symbol = fl_hashtable_get(program->symbols, ((struct ZirSymbolOperand*) temp_operand)->symbol->name);

        if (tmp_symbol == NULL)
            break;

        if (tmp_symbol->segment != ZENIT_NES_SEGMENT_TEMP)
            break;

        temp_operand = ((struct ZenitNesTempSymbol*) tmp_symbol)->source;
    }

    return NULL;
}

static inline struct ZenitNesSymbol* create_symbol(struct ZenitNesProgram *program, const char *name, struct ZirType *zir_type, enum ZenitNesSegment segment, uint16_t address)
{
    struct ZenitNesSymbol *nes_symbol = NULL;

    if (zir_type->typekind == ZIR_TYPE_ARRAY)
    {
        struct ZirArrayType *zir_array_type = (struct ZirArrayType*) zir_type;
        struct ZenitNesArraySymbol *array_symbol = fl_malloc(sizeof(struct ZenitNesArraySymbol));
        array_symbol->base.address = address;
        array_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
        array_symbol->base.segment = segment;
        array_symbol->base.symkind = ZENIT_NES_SYMBOL_ARRAY;
        array_symbol->base.size = zir_type_array_size(zir_array_type);
        array_symbol->elements = fl_array_new(sizeof(struct ZenitNesSymbol*), zir_array_type->length);

        size_t member_size = zir_type_size(zir_array_type->member_type);

        for (size_t i=0; i < zir_array_type->length; i++)
        {
            char buf[1024] = { 0 };
            size_t to_write = snprintf(NULL, 0, "%zu", i);
            snprintf(buf, 1024, "%zu", i);

            struct ZenitNesSymbol *element = create_symbol(program, buf, zir_array_type->member_type, segment, address + (member_size * i));
            array_symbol->elements[i] = element;
        }

        nes_symbol = (struct ZenitNesSymbol*) array_symbol;
    }
    else if (zir_type->typekind == ZIR_TYPE_STRUCT)
    {
        struct ZirStructType *zir_struct_type = (struct ZirStructType*) zir_type;
        struct ZenitNesStructSymbol *struct_symbol = fl_malloc(sizeof(struct ZenitNesStructSymbol));
        struct_symbol->base.address = address;
        struct_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
        struct_symbol->base.segment = segment;
        struct_symbol->base.symkind = ZENIT_NES_SYMBOL_STRUCT;
        struct_symbol->base.size = zir_type_struct_size(zir_struct_type);
        struct_symbol->members = fl_array_new(sizeof(struct ZenitNesSymbol*), 0);
        
        size_t members_offset = 0;
        struct FlListNode *zir_node = fl_list_head(zir_struct_type->members);
        while (zir_node)
        {
            struct ZirStructTypeMember *zir_member = (struct ZirStructTypeMember*) zir_node->value;

            struct ZenitNesSymbol *member_symbol = create_symbol(program, zir_member->name, zir_member->type, segment, address + members_offset);
            struct_symbol->members = fl_array_append(struct_symbol->members, &member_symbol);

            members_offset = zir_type_size(zir_member->type);

            zir_node = zir_node->next;
        }

        nes_symbol = (struct ZenitNesSymbol*) struct_symbol;
    }
    else if (zir_type->typekind == ZIR_TYPE_UINT)
    {
        struct ZirUintType *zir_uint_type = (struct ZirUintType*) zir_type;
        struct ZenitNesUintSymbol *uint_symbol = fl_malloc(sizeof(struct ZenitNesUintSymbol));
        uint_symbol->base.address = address;
        uint_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
        uint_symbol->base.segment = segment;
        uint_symbol->base.symkind = ZENIT_NES_SYMBOL_UINT;
        uint_symbol->base.size = zir_type_uint_size(zir_uint_type);
        
        nes_symbol = (struct ZenitNesSymbol*) uint_symbol;
    }
    else if (zir_type->typekind == ZIR_TYPE_REFERENCE)
    {
        struct ZirReferenceType *zir_reference_type = (struct ZirReferenceType*) zir_type;
        struct ZenitNesReferenceSymbol *ref_symbol = fl_malloc(sizeof(struct ZenitNesReferenceSymbol));
        ref_symbol->base.address = address;
        ref_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
        ref_symbol->base.segment = segment;
        ref_symbol->base.symkind = ZENIT_NES_SYMBOL_REFERENCE;
        ref_symbol->base.size = zir_type_reference_size(zir_reference_type);

        nes_symbol = (struct ZenitNesSymbol*) ref_symbol;
    }

    return nes_symbol;
}

static inline bool reserve_zp_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol **nes_symbol, struct ZirSymbol *zir_symbol, uint8_t *address)
{
    if (!program || !nes_symbol || !zir_symbol)
        return false;

    size_t max_size = 0xFF;

    // We need to get the symbol size to make sure it fits
    size_t symbol_size = zir_type_size(zir_symbol->type);
    
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


    *nes_symbol = create_symbol(program, zir_symbol->name, zir_symbol->type, ZENIT_NES_SEGMENT_ZP, slot);
    fl_hashtable_add(program->symbols, (*nes_symbol)->name, *nes_symbol);

    return true;
}

static inline bool reserve_data_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol **nes_symbol, struct ZirSymbol *zir_symbol, uint16_t *address)
{
    if (!program || !nes_symbol || !zir_symbol)
        return false;

    size_t max_size = fl_array_length(program->data.slots);

    // We need to get the symbol size to make sure it fits
    size_t symbol_size = zir_symbol->type->typekind == ZIR_TYPE_REFERENCE ? 2 /*bytes*/ : zir_type_size(zir_symbol->type);

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

    *nes_symbol = create_symbol(program, zir_symbol->name, zir_symbol->type, ZENIT_NES_SEGMENT_DATA, program->data.base_address + (uint16_t) slot);
    fl_hashtable_add(program->symbols, (*nes_symbol)->name, *nes_symbol);

    return true;
}

static inline bool reserve_temp_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol **nes_symbol, struct ZirSymbol *zir_symbol)
{
    if (!program || !nes_symbol || !zir_symbol)
        return false;

    // We need to get the symbol size to make sure it fits
    size_t symbol_size = zir_symbol->type->typekind == ZIR_TYPE_REFERENCE ? 2 /*bytes*/ : zir_type_size(zir_symbol->type);

    struct ZenitNesTempSymbol *temp_symbol = fl_malloc(sizeof(struct ZenitNesTempSymbol));

    temp_symbol->base.symkind = ZENIT_NES_SYMBOL_TEMP;
    temp_symbol->base.name = fl_cstring_dup(zir_symbol->name);
    temp_symbol->base.address = 0; // Mind that it being a temp symbol means we don't actually use the address
    temp_symbol->base.segment = ZENIT_NES_SEGMENT_TEMP;
    temp_symbol->base.size = symbol_size;

    //temp_symbol->base.elements = zir_symbol->type->typekind == ZIR_TYPE_ARRAY ? ((struct ZirArrayType*) zir_symbol->type)->length : 1;
    //temp_symbol->base.size = symbol_size;
    //temp_symbol->base.element_size = symbol_size / temp_symbol->base.elements;

    *nes_symbol = (struct ZenitNesSymbol*) temp_symbol;
    fl_hashtable_add(program->symbols, temp_symbol->base.name, *nes_symbol);

    return true;
}

static inline bool map_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol **nes_symbol, struct ZirSymbol *zir_symbol, uint16_t address)
{
    *nes_symbol = create_symbol(program, zir_symbol->name, zir_symbol->type, ZENIT_NES_SEGMENT_CODE, address);

    fl_hashtable_add(program->symbols, (*nes_symbol)->name, *nes_symbol);

    return true;
}


struct ZenitNesSymbol* zenit_nes_program_reserve_symbol(struct ZenitNesProgram *program, struct ZirBlock *block, ZirAttributeMap *attributes, struct ZirSymbol *zir_symbol)
{
    struct ZenitNesSymbol *nes_symbol = NULL;

    if (zir_symbol->name[0] == '%')
    {
        reserve_temp_symbol(program, &nes_symbol, zir_symbol);
        return nes_symbol;
    }
    
    bool is_global = block->type == ZIR_BLOCK_GLOBAL;

    if (attributes != NULL && zir_attribute_map_has_key(attributes, "NES"))
    {
        struct ZirAttribute *nes_attribute = zir_attribute_map_get(attributes, "NES");

        if (zir_property_map_has_key(nes_attribute->properties, "segment"))
        {
            struct ZirProperty *segment_property = zir_property_map_get(nes_attribute->properties, "segment");

            if (segment_property->value->type == ZIR_OPERAND_SYMBOL)
            {
                struct ZirSymbolOperand *symbol_operand = (struct ZirSymbolOperand*)segment_property->value;

                if (flm_cstring_equals(symbol_operand->symbol->name, "zp"))
                    reserve_zp_symbol(program, &nes_symbol, zir_symbol, NULL);
                else if (flm_cstring_equals(symbol_operand->symbol->name, "data"))
                    reserve_data_symbol(program, &nes_symbol, zir_symbol, NULL);
            }
        }
        else if (zir_property_map_has_key(nes_attribute->properties, "address"))
        {
            struct ZirProperty *address_property = zir_property_map_get(nes_attribute->properties, "address");
            struct ZirUintOperand *uint_value = NULL;

            if (address_property->value->type == ZIR_OPERAND_UINT)
            {
                uint_value = (struct ZirUintOperand*) address_property->value;
            }
            else if (address_property->value->type == ZIR_OPERAND_SYMBOL)
            {
                struct ZirSymbolOperand *symbol_operand = (struct ZirSymbolOperand*) address_property->value;

                struct ZenitNesSymbol *symbol = fl_hashtable_get(program->symbols, symbol_operand->symbol->name);

                struct ZirOperand *tmp = zenit_nes_program_get_tmpsym_operand(program, symbol, ZIR_OPERAND_UINT);

                if (tmp->type == ZIR_OPERAND_UINT)
                    uint_value = (struct ZirUintOperand*) tmp;
            }

            if (uint_value == NULL)
                return NULL;

            if (uint_value->type->size == ZIR_UINT_8)
            {
                reserve_zp_symbol(program, &nes_symbol, zir_symbol, &uint_value->value.uint8);
            }
            else if (uint_value->type->size == ZIR_UINT_16)
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
