#include <inttypes.h>
#include <fllib/Cstring.h>
#include "program.h"

Rp2a03Program* rp2a03_program_new(size_t data_base_address, size_t startup_base_address, size_t code_base_address)
{
    Rp2a03Program *program = fl_malloc(sizeof(Rp2a03Program));

    program->startup = rp2a03_text_segment_new(startup_base_address);
    program->code = rp2a03_text_segment_new(code_base_address);

    // Size: 1 bank NROM-256 or 2 banks NROM-128 (actually, mirrored)
    program->data = rp2a03_data_segment_new(data_base_address, 0x8000);

    return program;
}

void rp2a03_program_free(Rp2a03Program *program)
{
    if (!program)
        return;

    rp2a03_text_segment_free(program->code);
    rp2a03_text_segment_free(program->startup);
    rp2a03_data_segment_free(program->data);

    fl_free(program);
}

char* rp2a03_program_disassemble(Rp2a03Program *program)
{
    char *output = fl_cstring_new(0);

    output = rp2a03_data_segment_disassemble(program->data, true, output);
    output = rp2a03_text_segment_disassemble(program->startup, "STARTUP segment", output);
    output = rp2a03_text_segment_disassemble(program->code, "CODE segment", output);

    return output;
}

/*
uint16_t rp2a03_program_emit_label(Rp2a03TextSegment *code)
{
    return code->pc + program->base_address;
}

uint8_t rp2a03_program_calc_rel_addr(Rp2a03TextSegment *code, uint16_t address)
{
    if (address <= code->pc + program->base_address)
        return 256 - (code->pc + program->base_address + 2 - address); // 2 is for the 2-byte instruction

    return address - code->pc + program->base_address;
}
*/
void rp2a03_program_emit_abs(Rp2a03Program *program, Rp2a03TextSegment *code, Rp2a03Mnemonic opcode, uint16_t bytes)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = rp2a03_opcode_lookup(opcode, NES_ADDR_ABS);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = (uint8_t)(bytes);
    code->bytes[code->pc++] = (uint8_t)(bytes >> 8);    
}

void rp2a03_program_emit_abx(Rp2a03Program *program, Rp2a03TextSegment *code, Rp2a03Mnemonic opcode, uint16_t bytes)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = rp2a03_opcode_lookup(opcode, NES_ADDR_ABX);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = (uint8_t)(bytes);
    code->bytes[code->pc++] = (uint8_t)(bytes >> 8);
}

void rp2a03_program_emit_aby(Rp2a03Program *program, Rp2a03TextSegment *code, Rp2a03Mnemonic opcode, uint16_t bytes)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = rp2a03_opcode_lookup(opcode, NES_ADDR_ABY);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = (uint8_t)(bytes);
    code->bytes[code->pc++] = (uint8_t)(bytes >> 8);
}

void rp2a03_program_emit_imm(Rp2a03Program *program, Rp2a03TextSegment *code, Rp2a03Mnemonic opcode, uint8_t byte)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = rp2a03_opcode_lookup(opcode, NES_ADDR_IMM);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = byte;
}

void rp2a03_program_emit_imp(Rp2a03Program *program, Rp2a03TextSegment *code, Rp2a03Mnemonic opcode)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = rp2a03_opcode_lookup(opcode, NES_ADDR_IMP);
    code->bytes[code->pc++] = opcode_hex;
}

void rp2a03_program_emit_ind(Rp2a03Program *program, Rp2a03TextSegment *code, Rp2a03Mnemonic opcode, uint16_t bytes)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = rp2a03_opcode_lookup(opcode, NES_ADDR_IND);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = (uint8_t)(bytes);
    code->bytes[code->pc++] = (uint8_t)(bytes >> 8);
}

void rp2a03_program_emit_inx(Rp2a03Program *program, Rp2a03TextSegment *code, Rp2a03Mnemonic opcode, uint8_t byte)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = rp2a03_opcode_lookup(opcode, NES_ADDR_INX);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = byte;
}

void rp2a03_program_emit_iny(Rp2a03Program *program, Rp2a03TextSegment *code, Rp2a03Mnemonic opcode, uint8_t byte)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = rp2a03_opcode_lookup(opcode, NES_ADDR_INY);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = byte;
}

void rp2a03_program_emit_rel(Rp2a03Program *program, Rp2a03TextSegment *code, Rp2a03Mnemonic opcode, uint8_t byte)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = rp2a03_opcode_lookup(opcode, NES_ADDR_REL);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = byte;
}

void rp2a03_program_emit_zpg(Rp2a03Program *program, Rp2a03TextSegment *code, Rp2a03Mnemonic opcode, uint8_t byte)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = rp2a03_opcode_lookup(opcode, NES_ADDR_ZPG);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = byte;
}

void rp2a03_program_emit_zpx(Rp2a03Program *program, Rp2a03TextSegment *code, Rp2a03Mnemonic opcode, uint8_t byte)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = rp2a03_opcode_lookup(opcode, NES_ADDR_ZPX);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = byte;
}

void rp2a03_program_emit_zpy(Rp2a03Program *program, Rp2a03TextSegment *code, Rp2a03Mnemonic opcode, uint8_t byte)
{
    if (code->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = rp2a03_opcode_lookup(opcode, NES_ADDR_ZPY);
    code->bytes[code->pc++] = opcode_hex;
    code->bytes[code->pc++] = byte;
}
