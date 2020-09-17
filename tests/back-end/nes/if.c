#include <stdio.h>

#include <flut/flut.h>
#include "../../../src/front-end/type-check/check.h"
#include "../../../src/front-end/inference/infer.h"
#include "../../../src/front-end/parser/parse.h"
#include "../../../src/front-end/binding/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/codegen/zir.h"
#include "../../../src/back-end/nes/rp2a03/generate.h"
#include "../../../src/back-end/nes/ir/generate.h"
#include "tests.h"

void zenit_test_nes_conditionals(void)
{
    const char *zenit_source = 
        "if (true) {"                           "\n"
        "   #[NES(address: 0x00)] var zp = 1;"  "\n"
        "} else {"                              "\n"
        "   #[NES(address: 0x00)] var zp = 2;"  "\n"
        "}"                                     "\n"
        ""                                      "\n"
        "var b = true;"                         "\n"
        "if (b) {"                              "\n"
        "   #[NES(address: 0x8001)] var data = 8;"                      "\n"
        "} else {"                              "\n"
        "   #[NES(address: 0x8001)] var data = 5;"                      "\n"
        "}"                                     "\n"
    ;

    const char *rp2a03_disassembly = 
        "; RP2A03 PROGRAM DISASSEMBLY"                              "\n"
        "; STARTUP segment"                                         "\n"
        ""                                                          "\n"
        // true
        "8000:     LDA #$01"                                        "\n"
        // if (!true) jump to 800B (the else)
        "8002:     BEQ $07"                                         "\n"
        // var zp = 1
        "8004:     LDA #$01"                                        "\n"
        "8006:     STA $00"                                         "\n"
        // jump out of the if
        "8008:     JMP $800F"                                       "\n"
        // else { var zp = 2 }
        "800B:     LDA #$02"                                        "\n"
        "800D:     STA $00"                                         "\n"
        ""                                                          "\n"
    ;

    ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    flut_expect_compat("Parsing should not contain errors", zenit_parse_source(&ctx));
    flut_expect_compat("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    flut_expect_compat("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    flut_expect_compat("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    ZirProgram *zir_program = zenit_generate_zir(&ctx);
    ZnesProgram *znes_program = znes_generate_program(zir_program, true);
    Rp2a03Program *rp2a03_program = rp2a03_generate_program(znes_program);

    char *zir_program_dump_str = zir_program_dump(zir_program);
    printf("%s\n", zir_program_dump_str);
    fl_cstring_free(zir_program_dump_str);

    char *znes_program_dump_str = znes_program_dump(znes_program);
    printf("%s\n", znes_program_dump_str);
    fl_cstring_free(znes_program_dump_str);

    char *rp2a03_program_dump_str = rp2a03_program_disassemble(rp2a03_program);
    printf("%s\n", rp2a03_program_dump_str);
    printf("%d", flm_cstring_equals(rp2a03_program_dump_str, rp2a03_disassembly));


    fl_cstring_free(rp2a03_program_dump_str);

    rp2a03_program_free(rp2a03_program);
    znes_program_free(znes_program);
    zir_program_free(zir_program);
    zenit_context_free(&ctx);
}
