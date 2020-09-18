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
        "if (true) {"                                   "\n"
        "   #[NES(address: 0x00)] var zp = 1;"          "\n"
        "} else {"                                      "\n"
        "   #[NES(address: 0x00)] var zp = 2;"          "\n"
        "}"                                             "\n"
        ""                                              "\n"
        "var b = true;"                                 "\n"
        "if (b) {"                                      "\n"
        "   #[NES(address: 0x8001)] var data = 8;"      "\n"
        "} else {"                                      "\n"
        "   #[NES(address: 0x8001)] var data = 5;"      "\n"
        "}"                                             "\n"
        ""                                              "\n"
        "if (cast(true)) {"                             "\n"
        "   #[NES(address: 0x8002)] var data = 4;"      "\n"
        "}"
    ;

    const char *rp2a03_disassembly = 
        "; RP2A03 PROGRAM DISASSEMBLY"                              "\n"
        "; DATA segment hex dump"                                   "\n"
        ";"                                                         "\n"
        ";      | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F"  "\n"
        "; -----+------------------------------------------------"  "\n"
        "; 0000 | 00 00 00"                                         "\n"
        ""                                                          "\n"
        "; DATA segment as code"                                    "\n"
        ";"                                                         "\n"
        "8000:     BRK"                                             "\n"
        "8001:     BRK"                                             "\n"
        "8002:     BRK"                                             "\n"
        ""                                                          "\n"
        "; CODE segment"                                            "\n"
        ""                                                          "\n"
        // The "true" value
        "8003:     LDA #$01"                                        "\n"
        // if_false true jump to the "else" branch
        "8005:     BEQ $07"                                         "\n"
        // then branch: var zp = 1
        "8007:     LDA #$01"                                        "\n"
        "8009:     STA $00"                                         "\n"
        // then branch: jump out of the "then" branch skiping the "else"
        "800B:     JMP $8032"                                       "\n"
        // else branch: var zp = 2
        "800E:     LDA #$02"                                        "\n"
        "8010:     STA $00"                                         "\n"

        // var b = true
        "8012:     LDA #$01"                                        "\n"
        "8014:     STA $8000"                                       "\n"
        "8017:     LDA $8000"                                       "\n"
        // if_false b jump to the "else" branch
        "801A:     BEQ $08"                                         "\n"
        // then branch: var data = 8
        "801C:     LDA #$08"                                        "\n"
        "801E:     STA $8001"                                       "\n"
        // then branch: jump out of the "then" branch skiping the "else"
        "8021:     JMP $8032"                                       "\n"
        // else branch: var data = 5
        "8024:     LDA #$05"                                        "\n"
        "8026:     STA $8001"                                       "\n"

        // cast(true) value
        "8029:     LDA #$01"                                        "\n"
        // if_false cast(true) skip the "if"
        "802B:     BEQ $05"                                         "\n"
        // then branch: var data = 4
        "802D:     LDA #$04"                                        "\n"
        "802F:     STA $8002"                                       "\n"
        ""                                                          "\n"
    ;

    ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    flut_expect_compat("Parsing should not contain errors", zenit_parse_source(&ctx));
    flut_expect_compat("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    flut_expect_compat("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    flut_expect_compat("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    ZirProgram *zir_program = zenit_generate_zir(&ctx);

    ZnesContext *znes_context = znes_context_new(true);
    flut_expect_compat("NES IR should not contain errors", znes_generate_program(znes_context, zir_program));

    Rp2a03Program *rp2a03_program = rp2a03_generate_program(znes_context->program);

    char *rp2a03_program_dump_str = rp2a03_program_disassemble(rp2a03_program);
    flut_expect_compat("Disassemble of RP2A03 program must be equals to the handcrafted source", flm_cstring_equals(rp2a03_program_dump_str, rp2a03_disassembly));
    fl_cstring_free(rp2a03_program_dump_str);

    rp2a03_program_free(rp2a03_program);
    znes_context_free(znes_context);
    zir_program_free(zir_program);
    zenit_context_free(&ctx);
}
