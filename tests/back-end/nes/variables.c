#include <stdio.h>
#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/check.h"
#include "../../../src/front-end/infer.h"
#include "../../../src/front-end/parse.h"
#include "../../../src/front-end/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/zirgen.h"
#include "../../../src/back-end/nes/program.h"
#include "../../../src/back-end/nes/generate.h"
#include "tests.h"

void zenit_test_nes_literal_variables(void)
{
    const char *zenit_source = 
        "var a : uint8 = 1;"                                    "\n"
        "var b : uint8 = 2;"                                    "\n"
        "var c : uint16 = 0xDEAD;"                              "\n"
        "var d : uint16 = 0xBEEF;"                              "\n"
        "var e : uint16 = 0xAD;"                                "\n"
        "#[NES(address: 0x2)]"                                  "\n"
        "var f : uint16 = 0x7FF;"                               "\n"
        "#[NES(address: 0x4)]"                                  "\n"
        "var g : uint16 = 0xAF;"                                "\n"
        "#[NES(address: 0x2000)]"                               "\n"
        "var ppuctrl = 0;"                                      "\n"
        "var arr = [ 0, 1, 2 ];"                                "\n"
        "var arrarr = [ [ 3, 4 ], [ 5, 6 ], [ 7, 8 ] ];"        "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    struct ZirProgram *zir_program = zenit_generate_zir(&ctx);

    struct ZenitNesProgram *nes_program = zenit_nes_generate_program(zir_program);
    
    fl_expect("Data segment at 0x00 should be 0x1 (a)",             nes_program->data.bytes[0x00] == 0x1);
    fl_expect("Data segment at 0x01 should be 0x2 (b)",             nes_program->data.bytes[0x01] == 0x2);
    fl_expect("Data segment at 0x02 should be 0xAD (c lo)",         nes_program->data.bytes[0x02] == 0xAD);
    fl_expect("Data segment at 0x03 should be 0xDE (c hi)",         nes_program->data.bytes[0x03] == 0xDE);
    fl_expect("Data segment at 0x04 should be 0xEF (d lo)",         nes_program->data.bytes[0x04] == 0xEF);
    fl_expect("Data segment at 0x05 should be 0xBE (d hi)",         nes_program->data.bytes[0x05] == 0xBE);
    fl_expect("Data segment at 0x06 should be 0xAD (e lo)",         nes_program->data.bytes[0x06] == 0xAD);
    fl_expect("Data segment at 0x07 should be 0x00 (e hi)",         nes_program->data.bytes[0x07] == 0x00);
    fl_expect("Data segment at 0x08 should be 0x00 (arr[0])",       nes_program->data.bytes[0x08] == 0x00);
    fl_expect("Data segment at 0x09 should be 0x01 (arr[1])",       nes_program->data.bytes[0x09] == 0x01);
    fl_expect("Data segment at 0x0A should be 0x02 (arr[2])",       nes_program->data.bytes[0x0A] == 0x02);
    fl_expect("Data segment at 0x0B should be 0x03 (arrarr[0][0])", nes_program->data.bytes[0x0B] == 0x03);
    fl_expect("Data segment at 0x0C should be 0x04 (arrarr[0][1])", nes_program->data.bytes[0x0C] == 0x04);
    fl_expect("Data segment at 0x0D should be 0x05 (arrarr[1][0])", nes_program->data.bytes[0x0D] == 0x05);
    fl_expect("Data segment at 0x0E should be 0x06 (arrarr[1][1])", nes_program->data.bytes[0x0E] == 0x06);
    fl_expect("Data segment at 0x0F should be 0x07 (arrarr[2][0])", nes_program->data.bytes[0x0F] == 0x07);
    fl_expect("Data segment at 0x10 should be 0x08 (arrarr[2][1])", nes_program->data.bytes[0x10] == 0x08);

    zenit_nes_program_free(nes_program);
    zir_program_free(zir_program);
    zenit_context_free(&ctx);
}
