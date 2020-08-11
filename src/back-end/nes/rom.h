#ifndef NES_ROM_H
#define NES_ROM_H

#include <stdint.h>
#include "header.h"
#include "program.h"

typedef struct ZnesNrom128 {
    uint8_t bank0[0x4000];
    uint8_t bank1[0x3FFA];
    uint16_t nmi_addr;
    uint16_t res_addr;
    uint16_t irq_addr;
} ZnesNrom128;

typedef struct ZnesNrom256 {
    uint8_t bank[0x7FFA];
    uint16_t nmi_addr;
    uint16_t res_addr;
    uint16_t irq_addr;
} ZnesNrom256;

typedef struct ZnesRom {
    ZnesFileHeader header;
    ZnesNrom256 prg_rom;
} ZnesRom;

ZnesRom* znes_rom_new(ZnesProgram *program);
void znes_rom_free(ZnesRom *rom);
void znes_rom_dump(ZnesRom *rom, const char *filename);

#endif /* NES_ROM_H */
