#include <fllib.h>
#include "rom.h"

typedef uint8_t(*Nes1ByteInstruction)(enum NesOpcode opcode, enum NesAddressMode mode, uint16_t bytes);
typedef uint8_t(*Nes2BytesInstruction)(enum NesOpcode opcode, enum NesAddressMode mode, uint16_t bytes);
typedef uint8_t(*Nes3BytesInstruction)(enum NesOpcode opcode, enum NesAddressMode mode, uint16_t bytes);

struct NesRom nes_rom_new(uint16_t base_address)
{
    return (struct NesRom) {
        .pc = 0,
        .base_address = base_address,
        .header = {
            .magic = { 0x4E, 0x45, 0x53, 0x1A },
            .prg_rom = 2,
            .chr_rom = 1,
            .flag6 = 0,
            .flag7 = 0,
            .flag8 = 0,
            .flag9 = 0,
            .flag10 = 0,
            .unused = { 0 }
        },
        .prg_rom = (struct NesProgramRom) {
            .data = { 0 },
            .nmi_addr = 0,
            .res_addr = 0,
            .irq_addr = 0,
        }
    };
}

uint16_t nes_rom_emit_label(struct NesRom *rom)
{
    return rom->pc + rom->base_address;
}

uint8_t nes_rom_calc_rel_addr(struct NesRom *rom, uint16_t address)
{
    if (address <= rom->pc + rom->base_address)
        return 256 - (rom->pc + rom->base_address + 2 - address); // 2 is for the 2-byte instruction

    return address - rom->pc + rom->base_address;
}

void nes_rom_emit_abs(struct NesRom *rom, enum NesOpcode opcode, uint16_t bytes)
{
    if (rom->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_ABS);
    rom->prg_rom.data[rom->pc++] = opcode_hex;
    rom->prg_rom.data[rom->pc++] = (uint8_t)(bytes);
    rom->prg_rom.data[rom->pc++] = (uint8_t)(bytes >> 8);
}

void nes_rom_emit_abx(struct NesRom *rom, enum NesOpcode opcode, uint16_t bytes)
{
    if (rom->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_ABX);
    rom->prg_rom.data[rom->pc++] = opcode_hex;
    rom->prg_rom.data[rom->pc++] = (uint8_t)(bytes);
    rom->prg_rom.data[rom->pc++] = (uint8_t)(bytes >> 8);
}

void nes_rom_emit_aby(struct NesRom *rom, enum NesOpcode opcode, uint16_t bytes)
{
    if (rom->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_ABY);
    rom->prg_rom.data[rom->pc++] = opcode_hex;
    rom->prg_rom.data[rom->pc++] = (uint8_t)(bytes);
    rom->prg_rom.data[rom->pc++] = (uint8_t)(bytes >> 8);
}

void nes_rom_emit_imm(struct NesRom *rom, enum NesOpcode opcode, uint8_t byte)
{
    if (rom->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_IMM);
    rom->prg_rom.data[rom->pc++] = opcode_hex;
    rom->prg_rom.data[rom->pc++] = byte;
}

void nes_rom_emit_imp(struct NesRom *rom, enum NesOpcode opcode)
{
    if (rom->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_IMP);
    rom->prg_rom.data[rom->pc++] = opcode_hex;
}

void nes_rom_emit_ind(struct NesRom *rom, enum NesOpcode opcode, uint16_t bytes)
{
    if (rom->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_IND);
    rom->prg_rom.data[rom->pc++] = opcode_hex;
    rom->prg_rom.data[rom->pc++] = (uint8_t)(bytes);
    rom->prg_rom.data[rom->pc++] = (uint8_t)(bytes >> 8);
}

void nes_rom_emit_inx(struct NesRom *rom, enum NesOpcode opcode, uint8_t byte)
{
    if (rom->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_INX);
    rom->prg_rom.data[rom->pc++] = opcode_hex;
    rom->prg_rom.data[rom->pc++] = byte;
}

void nes_rom_emit_iny(struct NesRom *rom, enum NesOpcode opcode, uint8_t byte)
{
    if (rom->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_INY);
    rom->prg_rom.data[rom->pc++] = opcode_hex;
    rom->prg_rom.data[rom->pc++] = byte;
}

void nes_rom_emit_rel(struct NesRom *rom, enum NesOpcode opcode, uint8_t byte)
{
    if (rom->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_REL);
    rom->prg_rom.data[rom->pc++] = opcode_hex;
    rom->prg_rom.data[rom->pc++] = byte;
}

void nes_rom_emit_zpg(struct NesRom *rom, enum NesOpcode opcode, uint8_t byte)
{
    if (rom->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_ZPG);
    rom->prg_rom.data[rom->pc++] = opcode_hex;
    rom->prg_rom.data[rom->pc++] = byte;
}

void nes_rom_emit_zpx(struct NesRom *rom, enum NesOpcode opcode, uint8_t byte)
{
    if (rom->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_ZPX);
    rom->prg_rom.data[rom->pc++] = opcode_hex;
    rom->prg_rom.data[rom->pc++] = byte;
}

void nes_rom_emit_zpy(struct NesRom *rom, enum NesOpcode opcode, uint8_t byte)
{
    if (rom->pc == UINT16_MAX)
    {
        // FIXME: Handle overflow
        return;
    }

    // We lookup the actual hex code
    uint8_t opcode_hex = zenit_nes_opcode_lookup(opcode, NES_ADDR_ZPY);
    rom->prg_rom.data[rom->pc++] = opcode_hex;
    rom->prg_rom.data[rom->pc++] = byte;
}

void nes_rom_dump(struct NesRom *rom, const char *filename)
{
    FILE *file = fl_io_file_open(filename, "w+b");
    fl_io_file_write_bytes(file, sizeof(struct NesFileHeader), (const FlByte*)&rom->header);
    fl_io_file_write_bytes(file, sizeof(struct NesProgramRom), (const FlByte*)&rom->prg_rom);
    fl_io_file_close(file);
}
