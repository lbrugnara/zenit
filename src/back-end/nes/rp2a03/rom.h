#ifndef RP2A03_ROM_H
#define RP2A03_ROM_H

#include <stdint.h>
#include "header.h"
#include "program.h"

typedef struct Rp2a03Nrom128 {
    uint8_t bank0[0x4000];
    uint8_t bank1[0x3FFA];
    uint16_t nmi_addr;
    uint16_t res_addr;
    uint16_t irq_addr;
} Rp2a03Nrom128;

typedef struct Rp2a03Nrom256 {
    uint8_t bank[0x7FFA];
    uint16_t nmi_addr;
    uint16_t res_addr;
    uint16_t irq_addr;
} Rp2a03Nrom256;

typedef struct Rp2a03Rom {
    Rp2a03RomHeader header;
    Rp2a03Nrom256 prg_rom;
} Rp2a03Rom;

Rp2a03Rom* rp2a03_rom_new(Rp2a03Program *program);
void rp2a03_rom_free(Rp2a03Rom *rom);
void rp2a03_rom_dump(Rp2a03Rom *rom, const char *filename);

#endif /* RP2A03_ROM_H */
