#include "program.h"
#include "opcode.h"

void zenit_nes_symbol_free(struct ZenitNesSymbol *symbol)
{
    if (symbol->name)
        fl_cstring_free(symbol->name);

    fl_free(symbol);
}

struct ZenitNesProgram* zenit_nes_program_new()
{
    struct ZenitNesProgram *program = fl_malloc(sizeof(struct ZenitNesProgram));

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

    program->init = (struct ZenitNesCodeSegment) {
        .pc = 0,
        .bytes = fl_array_new(sizeof(uint8_t), UINT16_MAX),
    };

    program->data = (struct ZenitNesDataSegment){
        .base_address = 0x8000,
        .slots = fl_array_new(sizeof(uint8_t), 0x7FFF),
        .bytes = fl_array_new(sizeof(uint8_t), 0x7FFF),
    };

    return program;
}

void zenit_nes_program_free(struct ZenitNesProgram *program)
{
    if (!program)
        return;

    fl_array_free(program->code.bytes);

    fl_hashtable_free(program->symbols);

    fl_array_free(program->init.bytes);

    fl_array_free(program->data.slots);
    fl_array_free(program->data.bytes);

    fl_free(program);
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
