#include <stdio.h>
#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/passes/check.h"
#include "../../../src/front-end/passes/infer.h"
#include "../../../src/front-end/passes/parse.h"
#include "../../../src/front-end/passes/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/passes/zirgen.h"
#include "../../../src/back-end/nes/program.h"
#include "../../../src/back-end/nes/generate.h"
#include "tests.h"

void zenit_test_nes_program(void)
{
    const char *zenit_source = 
        "#[NES(address: 0x8000)]"                               "\n"
        "var reset = ["                                         "\n"
        "    0x78,                      // SEI"                 "\n"
        "    0xD8,                      // CLD"                 "\n"
        "    0xA2, 0x40,                // LDX #$40"            "\n"
        "    0x8E, 0x17, 0x40,          // STX $4017"           "\n"
        "    0xA2, 0xFF,                // LDX #$FF"            "\n"
        "    0x9A,                      // TXS"                 "\n"
        "    0xE8,                      // INX"                 "\n"
        "    0x8E, 0x00, 0x20,          // STX $2000"           "\n"
        "    0x8E, 0x01, 0x20,          // STX $2001"           "\n"
        "    0x8E, 0x10, 0x40,          // STX $4010"           "\n"
        "    0x2C, 0x02, 0x20,          // BIT $2002"           "\n"
        "    0x10, 0xFB,                // BPL $0014"           "\n"
        "    0x2C, 0x02, 0x20,          // BIT $2002"           "\n"
        "    0x10, 0xFB,                // BPL $0019"           "\n"
        "    0xA9, 0x40,                // LDA #$40"            "\n"
        "    0x8D, 0x01, 0x20,          // STA $2001"           "\n"
        "    0x4C, 0x23, 0x80,          // JMP $8023"           "\n"
        "];"                                                    "\n"
        ""                                                      "\n"
        "#[NES(address: 0x8026)]"                               "\n"
        "var nmi = ["                                           "\n"
        "    0x40,                      // RTI"                 "\n"
        "];"                                                    "\n"
        ""                                                      "\n"
        "#[NES(address: 0x8027)]"                               "\n"
        "var irq = ["                                           "\n"
        "    0x40,                      // RTI"                 "\n"
        "];"                                                    "\n"
        ""                                                      "\n"
        "#[NES(address: 0xFFFA)]"                               "\n"
        "var vectors : [3]uint16 = ["                           "\n"
        "    0x8026,                    // NMI"                 "\n"
        "    0x8000,                    // Reset"               "\n"
        "    0x8027,                    // IRQ"                 "\n"
        "];"                                                    "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    struct ZirProgram *zir_program = zenit_generate_zir(&ctx);

    struct ZenitNesProgram *nes_program = zenit_nes_generate_program(zir_program);

    zenit_nes_program_free(nes_program);
    zir_program_free(zir_program);
    zenit_context_free(&ctx);
}
