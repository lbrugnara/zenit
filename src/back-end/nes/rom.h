#ifndef ZENIT_NES_ROM_H
#define ZENIT_NES_ROM_H

#include <stdint.h>
#include "header.h"
#include "opcode.h"

struct NesProgramRom {
    uint8_t data[32762];
    uint16_t nmi_addr;
    uint16_t res_addr;
    uint16_t irq_addr;
};

struct NesRom {
    uint16_t pc;
    uint16_t base_address;
    struct NesFileHeader header;
    struct NesProgramRom prg_rom;
};

struct NesRom nes_rom_new(uint16_t base_address);
void nes_rom_dump(struct NesRom *rom, const char *filename);

uint16_t nes_rom_emit_label(struct NesRom *rom);
uint8_t nes_rom_calc_rel_addr(struct NesRom *rom, uint16_t address);
void nes_rom_emit_abs(struct NesRom *rom, enum NesOpcode opcode, uint16_t bytes);
void nes_rom_emit_abx(struct NesRom *rom, enum NesOpcode opcode, uint16_t bytes);
void nes_rom_emit_aby(struct NesRom *rom, enum NesOpcode opcode, uint16_t bytes);
void nes_rom_emit_imm(struct NesRom *rom, enum NesOpcode opcode, uint8_t byte);
void nes_rom_emit_imp(struct NesRom *rom, enum NesOpcode opcode);
void nes_rom_emit_ind(struct NesRom *rom, enum NesOpcode opcode, uint16_t bytes);
void nes_rom_emit_inx(struct NesRom *rom, enum NesOpcode opcode, uint8_t byte);
void nes_rom_emit_iny(struct NesRom *rom, enum NesOpcode opcode, uint8_t byte);
void nes_rom_emit_rel(struct NesRom *rom, enum NesOpcode opcode, uint8_t byte);
void nes_rom_emit_zpg(struct NesRom *rom, enum NesOpcode opcode, uint8_t byte);
void nes_rom_emit_zpx(struct NesRom *rom, enum NesOpcode opcode, uint8_t byte);
void nes_rom_emit_zpy(struct NesRom *rom, enum NesOpcode opcode, uint8_t byte);

#endif /* ZENIT_NES_ROM_H */
