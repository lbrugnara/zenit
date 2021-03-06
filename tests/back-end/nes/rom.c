#include <stdio.h>


#include <flut/flut.h>
#include "../../../src/front-end/type-check/check.h"
#include "../../../src/front-end/inference/infer.h"
#include "../../../src/front-end/parser/parse.h"
#include "../../../src/front-end/binding/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/codegen/zir.h"
#include "../../../src/back-end/nes/ir/generate.h"
#include "../../../src/back-end/nes/rp2a03/generate.h"
#include "../../../src/back-end/nes/rp2a03/rom.h"
#include "tests.h"

void zenit_test_nes_rom(void)
{
    const char *zenit_source = 
        "/**"   "\n"
        " *  Each array variable contains assembled instructions for the NES CPU. By means of the special 'NES' attribute"                          "\n"
        " *  and particularly with the 'address' property we ask Zenit's NES backend to store each array in certain addresses:"                     "\n"
        " *"                                                                                                                                        "\n"
        " *  - The 'bg_color' initialized to 0x20 is a red background, changing it to 0x40 should make it green."                                   "\n"
        " *  - The 'reset' array is the routine called when the reset interrupt occurs. It prepares the PPU and then loops forever."                "\n"
        " *  - The 'nmi' array is the routine called when a non-maskable interrupt occurs. This routine changes the background color on each call." "\n"
        " *  - The 'vectors' array tells the CPU to use addresses 0x8000 and 0x8017 for the reset and NMI interrupts handler, respectively."        "\n"
        " */"                                                                                                                                       "\n"
        ""                                                                                                                                          "\n"
        "#[NES(address: 0x00)]           // bg_color is stored in the zero page"                                                                    "\n"
        "var bg_color = 0x20;"                                                                                                                      "\n"
        ""                                                                                                                                          "\n"
        "#[NES(address: 0x8000)]         // Reset handler"                                                                                          "\n"
        "var reset = ["                                                                                                                             "\n"
        "    0x78,                       // SEI           Disable IRQs"                                                                             "\n"
        "    0xD8,                       // CLD           Disable decimal mode"                                                                     "\n"
        "    0xA2, 0x00,                 // LDX #$00"                                                                                               "\n"
        "    0x2C, 0x02, 0x20,           // BIT $2002     vwait1"                                                                                   "\n"
        "    0x10, 0xFB,                 // BPL $0014"                                                                                              "\n"
        "    0x2C, 0x02, 0x20,           // BIT $2002     vwait2"                                                                                   "\n"
        "    0x10, 0xFB,                 // BPL $0014"                                                                                              "\n"
        "    0xA9, 0x88,                 // LDA #$88      Enable NMI"                                                                               "\n"
        "    0x8D, 0x00, 0x20,           // STA $2000"                                                                                              "\n"
        "    0x4C, 0x13, 0x80,           // JMP $8013     Jump forever"                                                                             "\n"
        "];"                                                                                                                                        "\n"
        ""                                                                                                                                          "\n"
        "#[NES(address: 0x8016)]         // NMI handler"                                                                                            "\n"
        "var nmi : []uint8 = ["                                                                                                                     "\n"
        "    0xA6, cast(&bg_color),      // LDX $00       Load bg_color to X"                                                                       "\n"
        "    0xE8,                       // INX           Increment X"                                                                              "\n"
        "    0x86, 0x00,                 // STX $00       Store X to bg_color"                                                                      "\n"
        "    0x8E, 0x01, 0x20,           // STX $2001     Write PPUMASK"                                                                            "\n"
        "    0x40,                       // RTI"                                                                                                    "\n"
        "];"                                                                                                                                        "\n"
        ""                                                                                                                                          "\n"
        "#[NES(address: 0xFFFA)]"                                                                                                                   "\n"
        "var vectors : []uint16 = ["                                                                                                                "\n"
        "    cast(&nmi),                 // NMI"                                                                                                    "\n"
        "    cast(&reset),               // Reset"                                                                                                  "\n"
        "];"                                                                                                                                        "\n"
    ;

    ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    flut_expect_compat("Parsing should not contain errors", zenit_parse_source(&ctx));
    flut_expect_compat("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    flut_expect_compat("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    flut_expect_compat("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    ZirProgram *zir_program = zenit_generate_zir(&ctx);

    ZnesContext *znes_context = znes_context_new(false);
    flut_expect_compat("NES IR should not contain errors", znes_generate_program(znes_context, zir_program));

    Rp2a03Program *rp2a03_program = rp2a03_generate_program(znes_context->program);

    flut_expect_compat("RP2A03 program must be valid", rp2a03_program != NULL);

    Rp2a03Rom *nes_rom = rp2a03_rom_new(rp2a03_program);

    flut_expect_compat("NES ROM must be valid", nes_rom != NULL);

    const uint8_t data[] = { 0x78, 0xd8, 0xa2, 0x00, 0x2c, 0x02, 0x20, 0x10, 0xfb, 0x2c, 0x02, 0x20, 0x10, 0xfb, 0xa9, 0x88, 0x8d, 0x00, 0x20, 0x4c, 0x13, 0x80, 0xa6, 0x00, 0xe8, 0x86, 0x00, 0x8e, 0x01, 0x20, 0x40 };
    const uint8_t startup[] = { 0xa9, 0x20, 0x85, 0x00 , 0x4c, 0x00, 0x80 }; // NOTE: The startup routine contains as its last instruction a JMP to the -original- RESET vector
    const uint8_t vectors[] = { 0x16, 0x80, 0x1f, 0x80, 0x00, 0x00 }; // NOTE: The RESET address points to the startup routine

    bool data_equals = memcmp(data, nes_rom->prg_rom.bank, sizeof(data) / sizeof(data[0])) == 0;
    bool startup_equals = memcmp(startup, nes_rom->prg_rom.bank + sizeof(data) / sizeof(data[0]), sizeof(startup) / sizeof(startup[0])) == 0;
    // It's a "hack", we compare the 3 x uint16_t that are the nmi_addr, res_addr, and irq_addr that are placed at the end of the Rp2a03Nrom256
    bool vectors_equals = memcmp(vectors, nes_rom->prg_rom.bank + sizeof(nes_rom->prg_rom.bank), sizeof(vectors) / sizeof(vectors[0])) == 0;

    flut_expect_compat("DATA segment must be equals to the precomputed value", data_equals);
    flut_expect_compat("STARTUP segment must be equals to the precomputed value", startup_equals);
    flut_expect_compat("Vectors must be equals to the precomputed value", vectors_equals);

    rp2a03_rom_free(nes_rom);
    rp2a03_program_free(rp2a03_program);
    znes_context_free(znes_context);
    zir_program_free(zir_program);
    zenit_context_free(&ctx);
}
