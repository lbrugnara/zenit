#ifndef ZENIT_NES_ROM_H
#define ZENIT_NES_ROM_H

#include <stdint.h>
#include <fllib.h>

struct ZenitNesSymbol {
    char *name;
    size_t size;
    uint16_t address;
};

struct ZenitNesRomTextSection {
    uint8_t *content;
};

struct ZenitNesRomZeroPageSection {
    FlHashtable symbols;
    uint16_t slot;
};

struct ZenitNesRomDataSection {
    FlHashtable symbols;
    uint16_t slot;
};

struct ZenitNesRom {
    struct ZenitNesRomTextSection text;
    struct ZenitNesRomZeroPageSection zp;
    struct ZenitNesRomDataSection data;
};

struct ZenitNesRom* zenit_nes_rom_new(void);

#endif /* ZENIT_NES_ROM_H */
