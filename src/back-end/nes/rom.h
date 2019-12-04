#ifndef NES_ROM_H
#define NES_ROM_H

#include <stdint.h>
#include "header.h"
#include "program.h"

struct ZenitNesNrom128 {
    uint8_t bank0[0x4000];
    uint8_t bank1[0x3FFA];
    uint16_t nmi_addr;
    uint16_t res_addr;
    uint16_t irq_addr;
};

struct ZenitNesNrom256 {
    uint8_t bank[0x7FFA];
    uint16_t nmi_addr;
    uint16_t res_addr;
    uint16_t irq_addr;
};

struct ZenitNesRom {
    struct ZenitNesFileHeader header;
    struct ZenitNesNrom256 prg_rom;
};

struct ZenitNesRom* zenit_nes_rom_new(struct ZenitNesProgram *program);
void zenit_nes_rom_free(struct ZenitNesRom *rom);
void zenit_nes_rom_dump(struct ZenitNesRom *rom, const char *filename);

#endif /* NES_ROM_H */
