#ifndef ZENIT_NES_HEADER_H
#define ZENIT_NES_HEADER_H

#include <stdint.h>

typedef struct ZnesFileHeader {
    uint8_t magic[4]; // $4E $45 $53 $1A ("NES" followed by MS-DOS end-of-file)
    uint8_t prg_rom; // Size of PRG-ROM in 16KB units
    uint8_t chr_rom; // Size of CHR-ROM in 8KB units (0 means the board uses CHR-RAM)

    union {
        uint8_t raw;
        struct {
            uint8_t mapper_low  : 4;    // Lower nybble of mapper number
            uint8_t vram_4s     : 1;    // 1: Ignore mirroring control or above mirroring bit; instead provide four-screen VRAM
            uint8_t trainer     : 1;    // 1: 512-byte trainer at $7000-$71FF (stored before PRG data)
            uint8_t battery     : 1;    // 1: Cartridge contains battery-backed PRG RAM ($6000-7FFF) or other persistent memory
            uint8_t mirroring   : 1;    // Mirroring: 0: horizontal (vertical arrangement) (CIRAM A10 = PPU A11)
        } flags;                        //            1: vertical (horizontal arrangement) (CIRAM A10 = PPU A10)
    } flag6;

    union {
        uint8_t raw;
        struct {
            uint8_t mapper_hi   : 4;    // Upper nybble of mapper number
            uint8_t nes2        : 2;    // If equal to 2, flags 8-15 are in NES 2.0 format
            uint8_t playchoice  : 1;    // PlayChoice-10 (8KB of Hint Screen data stored after CHR data)
            uint8_t vsu         : 1;    // VS Unisystem
        } flags;
    } flag7;

    uint8_t flag8;      // PRG-RAM size (rarely used extension)

    union {
        uint8_t raw;
        struct {
            uint8_t reserved    : 7;    // Reserved, set to zero
            uint8_t tv_system   : 1;    // TV system (0: NTSC; 1: PAL)
        } flags;
    } flag9;            // TV system (rarely used extension)

    union {
        uint8_t raw;
        struct {
            uint8_t unused          : 2;
            uint8_t bus_conflict    : 1;    // 0: Board has no bus conflicts; 1: Board has bus conflicts
            uint8_t prgram          : 1;    // PRG RAM ($6000-$7FFF) (0: present; 1: not present)
            uint8_t unused2         : 2;
            uint8_t tv_system       : 1;    // TV system (0: NTSC; 2: PAL; 1/3: dual compatible)
        } flags;
    } flag10;           // TV system, PRG-RAM presence (unofficial, rarely used extension)

    // 11-15: Unused padding (should be filled with zero, but some rippers put their name across bytes 7-15)
    uint8_t unused[5];
} ZnesFileHeader;

#endif /* ZENIT_NES_HEADER_H */
