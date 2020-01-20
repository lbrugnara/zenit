#include <stdio.h>
#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/phases/check.h"
#include "../../../src/front-end/phases/infer.h"
#include "../../../src/front-end/phases/parse.h"
#include "../../../src/front-end/phases/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/phases/zirgen.h"
#include "../../../src/back-end/nes/program.h"
#include "../../../src/back-end/nes/generate.h"
#include "tests.h"

void zenit_test_nes_global_vars(void)
{
    const char *zenit_source = 
        "var a : uint8 = 1;"                                            "\n"

        "var b : uint8 = 2;"                                            "\n"

        "var c : uint16 = 0xDEAD;"                                      "\n"

        "var d : uint16 = 0xBEEF;"                                      "\n"

        "var e : uint16 = 0xAD;"                                        "\n"

        "#[NES(address: 0x2)]"                                          "\n"
        "var f : uint16 = 0x7FF;"                                       "\n"

        "#[NES(address: 0x4)]"                                          "\n"
        "var g : uint16 = 0xAF;"                                        "\n"

        "#[NES(address: 0x2000)]"                                       "\n"
        "var ppuctrl = 0;"                                              "\n"

        "var arr = [ 0, 1, 2 ];"                                        "\n"

        "var aarr = [ [ 3, 4 ], [ 5, 6 ], [ 7, 8 ] ];"                  "\n"
        
        "var a_ref = &a;"                                               "\n"
        "var b_ref = &b;"                                               "\n"
        "var aarr_ref = &aarr;"                                         "\n"
        "var aarr_ref_ref = &aarr_ref;"                                 "\n"
        "var ppuctrl_ref = &ppuctrl;"                                   "\n"

        "var aaarr = [ [ [ 3, 4 ] ], [ [ 5, 6 ] ], [ [ 7, 8 ] ] ];"     "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    struct ZirProgram *zir_program = zenit_generate_zir(&ctx);

    struct ZenitNesProgram *nes_program = zenit_nes_generate_program(zir_program);
    
    fl_expect("Data segment at 0x00 should be 0x1 (a)",                 nes_program->data.bytes[0x00] == 0x1);
    fl_expect("Data segment at 0x01 should be 0x2 (b)",                 nes_program->data.bytes[0x01] == 0x2);
    fl_expect("Data segment at 0x02 should be 0xAD (c lo)",             nes_program->data.bytes[0x02] == 0xAD);
    fl_expect("Data segment at 0x03 should be 0xDE (c hi)",             nes_program->data.bytes[0x03] == 0xDE);
    fl_expect("Data segment at 0x04 should be 0xEF (d lo)",             nes_program->data.bytes[0x04] == 0xEF);
    fl_expect("Data segment at 0x05 should be 0xBE (d hi)",             nes_program->data.bytes[0x05] == 0xBE);
    fl_expect("Data segment at 0x06 should be 0xAD (e lo)",             nes_program->data.bytes[0x06] == 0xAD);
    fl_expect("Data segment at 0x07 should be 0x00 (e hi)",             nes_program->data.bytes[0x07] == 0x00);
    fl_expect("Data segment at 0x08 should be 0x00 (arr[0])",           nes_program->data.bytes[0x08] == 0x00);
    fl_expect("Data segment at 0x09 should be 0x01 (arr[1])",           nes_program->data.bytes[0x09] == 0x01);
    fl_expect("Data segment at 0x0A should be 0x02 (arr[2])",           nes_program->data.bytes[0x0A] == 0x02);
    fl_expect("Data segment at 0x0B should be 0x03 (aarr[0][0])",       nes_program->data.bytes[0x0B] == 0x03);
    fl_expect("Data segment at 0x0C should be 0x04 (aarr[0][1])",       nes_program->data.bytes[0x0C] == 0x04);
    fl_expect("Data segment at 0x0D should be 0x05 (aarr[1][0])",       nes_program->data.bytes[0x0D] == 0x05);
    fl_expect("Data segment at 0x0E should be 0x06 (aarr[1][1])",       nes_program->data.bytes[0x0E] == 0x06);
    fl_expect("Data segment at 0x0F should be 0x07 (aarr[2][0])",       nes_program->data.bytes[0x0F] == 0x07);
    fl_expect("Data segment at 0x10 should be 0x08 (aarr[2][1])",       nes_program->data.bytes[0x10] == 0x08);
    fl_expect("Data segment at 0x11 should be 0x00 (a_ref lo)",         nes_program->data.bytes[0x11] == 0x00);
    fl_expect("Data segment at 0x12 should be 0x80 (a_ref hi)",         nes_program->data.bytes[0x12] == 0x80);
    fl_expect("Data segment at 0x13 should be 0x01 (b_ref lo)",         nes_program->data.bytes[0x13] == 0x01);
    fl_expect("Data segment at 0x14 should be 0x80 (b_ref hi)",         nes_program->data.bytes[0x14] == 0x80);
    fl_expect("Data segment at 0x15 should be 0x0B (aarr_ref lo)",      nes_program->data.bytes[0x15] == 0x0B);
    fl_expect("Data segment at 0x16 should be 0x80 (aarr_ref hi)",      nes_program->data.bytes[0x16] == 0x80);
    fl_expect("Data segment at 0x17 should be 0x15 (aarr_ref_ref lo)",  nes_program->data.bytes[0x17] == 0x15);
    fl_expect("Data segment at 0x18 should be 0x80 (aarr_ref_ref hi)",  nes_program->data.bytes[0x18] == 0x80);
    fl_expect("Data segment at 0x19 should be 0x00 (ppuctrl_ref lo)",   nes_program->data.bytes[0x19] == 0x00);
    fl_expect("Data segment at 0x1A should be 0x20 (ppuctrl_ref hi)",   nes_program->data.bytes[0x1A] == 0x20);
    fl_expect("Data segment at 0x1B should be 0x03 (aarr[0][0][0])",    nes_program->data.bytes[0x1B] == 0x03);
    fl_expect("Data segment at 0x1C should be 0x04 (aarr[0][0][1])",    nes_program->data.bytes[0x1C] == 0x04);
    fl_expect("Data segment at 0x1D should be 0x05 (aarr[1][0][0])",    nes_program->data.bytes[0x1D] == 0x05);
    fl_expect("Data segment at 0x1E should be 0x06 (aarr[1][0][1])",    nes_program->data.bytes[0x1E] == 0x06);
    fl_expect("Data segment at 0x1F should be 0x07 (aarr[2][0][0])",    nes_program->data.bytes[0x1F] == 0x07);
    fl_expect("Data segment at 0x20 should be 0x08 (aarr[2][0][1])",    nes_program->data.bytes[0x20] == 0x08);

    zenit_nes_program_free(nes_program);
    zir_program_free(zir_program);
    zenit_context_free(&ctx);
}

void zenit_test_nes_global_vars_zp(void)
{
    const char *zenit_source = 
        // ZP
        "#[NES(address: 0x00)]"
        "var zpvar : uint8 = 1;"                                    "\n"

        // ZP to DATA
        "var datavar = zpvar;"                                      "\n"

        // ZP to CODE
        "#[NES(address: 0x2000)]"
        "var codevar = zpvar;"                                      "\n"

        // ZP to TEMP (to DATA)
        "var tempvar = cast(zpvar : uint16);"                       "\n"

        // ZP to ZP
        "#[NES(address: 0x01)]"
        "var zpvar2 = zpvar;"                                       "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    struct ZirProgram *zir_program = zenit_generate_zir(&ctx);

    struct ZenitNesProgram *nes_program = zenit_nes_generate_program(zir_program);

    fl_expect("Data segment at 0x00 should be 0x00 (datavar -bss-)",    nes_program->data.bytes[0x00] == 0x00);
    fl_expect("Data segment at 0x01 should be 0x00 (tempvar -bss-)",    nes_program->data.bytes[0x01] == 0x00);
    
    // zpvar = 1
    fl_expect("Startup routine at 0x00 should be 0xA9 (LDA)",           nes_program->startup.bytes[0x00] == 0xA9);
    fl_expect("Startup routine at 0x01 should be 0x01 (#$01)",          nes_program->startup.bytes[0x01] == 0x01);
    fl_expect("Startup routine at 0x02 should be 0x85 (STA)",           nes_program->startup.bytes[0x02] == 0x85);
    fl_expect("Startup routine at 0x03 should be 0x00 ($00)",           nes_program->startup.bytes[0x03] == 0x00);
    
    // datavar = zpvar
    fl_expect("Startup routine at 0x04 should be 0xA5 (LDA)",           nes_program->startup.bytes[0x04] == 0xA5);
    fl_expect("Startup routine at 0x05 should be 0x00 ($00)",           nes_program->startup.bytes[0x05] == 0x00);
    fl_expect("Startup routine at 0x06 should be 0x8D (STA)",           nes_program->startup.bytes[0x06] == 0x8D);
    fl_expect("Startup routine at 0x07 should be 0x00 ($8000 lo)",      nes_program->startup.bytes[0x07] == 0x00);
    fl_expect("Startup routine at 0x08 should be 0x80 ($8000 hi)",      nes_program->startup.bytes[0x08] == 0x80);

    // codevar = zpvar
    fl_expect("Startup routine at 0x09 should be 0xA5 (LDA)",           nes_program->startup.bytes[0x09] == 0xA5);
    fl_expect("Startup routine at 0x0A should be 0x00 ($00)",           nes_program->startup.bytes[0x0A] == 0x00);
    fl_expect("Startup routine at 0x0B should be 0x8D (STA)",           nes_program->startup.bytes[0x0B] == 0x8D);
    fl_expect("Startup routine at 0x0C should be 0x00 ($2000 lo)",      nes_program->startup.bytes[0x0C] == 0x00);
    fl_expect("Startup routine at 0x0D should be 0x20 ($2000 hi)",      nes_program->startup.bytes[0x0D] == 0x20);

    // tempvar = cast(zpvar : uint16)
    fl_expect("Startup routine at 0x0E should be 0xA5 (LDA)",           nes_program->startup.bytes[0x0E] == 0xA5);
    fl_expect("Startup routine at 0x0F should be 0x00 ($00)",           nes_program->startup.bytes[0x0F] == 0x00);
    fl_expect("Startup routine at 0x10 should be 0x8D (STA)",           nes_program->startup.bytes[0x10] == 0x8D);
    fl_expect("Startup routine at 0x11 should be 0x00 ($8001 lo)",      nes_program->startup.bytes[0x11] == 0x01);
    fl_expect("Startup routine at 0x12 should be 0x80 ($8001 hi)",      nes_program->startup.bytes[0x12] == 0x80);
    fl_expect("Startup routine at 0x13 should be 0xA9 (LDA)",           nes_program->startup.bytes[0x13] == 0xA9);
    fl_expect("Startup routine at 0x14 should be 0x00 (#$00)",          nes_program->startup.bytes[0x14] == 0x00);
    fl_expect("Startup routine at 0x15 should be 0x8D (STA)",           nes_program->startup.bytes[0x15] == 0x8D);
    fl_expect("Startup routine at 0x16 should be 0x01 ($8002 lo)",      nes_program->startup.bytes[0x16] == 0x02);
    fl_expect("Startup routine at 0x17 should be 0x80 ($8002 hi)",      nes_program->startup.bytes[0x17] == 0x80);

    // zpvar2 = zpvar
    fl_expect("Startup routine at 0x18 should be 0xA5 (LDA)",           nes_program->startup.bytes[0x18] == 0xA5);
    fl_expect("Startup routine at 0x19 should be 0x00 ($00)",           nes_program->startup.bytes[0x19] == 0x00);
    fl_expect("Startup routine at 0x1A should be 0x85 (STA)",           nes_program->startup.bytes[0x1A] == 0x85);
    fl_expect("Startup routine at 0x1B should be 0x01 ($01)",           nes_program->startup.bytes[0x1B] == 0x01);

    zenit_nes_program_free(nes_program);
    zir_program_free(zir_program);
    zenit_context_free(&ctx);
}

void zenit_test_nes_global_vars_data(void)
{
    const char *zenit_source =         

        // DATA
        "var datavar = 1;"                                      "\n"

        // DATA to ZP
        "#[NES(address: 0x00)]"
        "var zpvar = datavar;"                                  "\n"

        // DATA to CODE
        "#[NES(address: 0x2000)]"
        "var codevar = datavar;"                                "\n"

        // DATA to TEMP (to DATA)
        "var tempvar = cast(datavar : uint16);"                 "\n"

        // DATA to DATA
        "var datavar2 = datavar;"                               "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    struct ZirProgram *zir_program = zenit_generate_zir(&ctx);

    struct ZenitNesProgram *nes_program = zenit_nes_generate_program(zir_program);

    // datavar = 1
    fl_expect("Data segment at 0x00 should be 0x01 (datavar)",    nes_program->data.bytes[0x00] == 0x01);
    
    // zpvar = datavar (static_context == true -> use LDA #$01 instead of using the abs addressing)
    fl_expect("Startup routine at 0x00 should be 0xA9 (LDA)",           nes_program->startup.bytes[0x00] == 0xA9);
    fl_expect("Startup routine at 0x01 should be 0x01 (#$01)",          nes_program->startup.bytes[0x01] == 0x01);
    fl_expect("Startup routine at 0x02 should be 0x85 (STA)",           nes_program->startup.bytes[0x02] == 0x85);
    fl_expect("Startup routine at 0x03 should be 0x00 ($00)",           nes_program->startup.bytes[0x03] == 0x00);
    
    // codevar = datavar
    fl_expect("Startup routine at 0x04 should be 0xAD (LDA)",           nes_program->startup.bytes[0x04] == 0xAD);
    fl_expect("Startup routine at 0x05 should be 0x00 ($8000 lo)",      nes_program->startup.bytes[0x05] == 0x00);
    fl_expect("Startup routine at 0x06 should be 0x80 ($8000 hi)",      nes_program->startup.bytes[0x06] == 0x80);
    fl_expect("Startup routine at 0x07 should be 0x8D (STA)",           nes_program->startup.bytes[0x07] == 0x8D);
    fl_expect("Startup routine at 0x08 should be 0x00 ($2000 lo)",      nes_program->startup.bytes[0x08] == 0x00);
    fl_expect("Startup routine at 0x09 should be 0x20 ($2000 hi)",      nes_program->startup.bytes[0x09] == 0x20);

    // tempvar = cast(datavar : uint16)
    fl_expect("Data segment at 0x01 should be 0x01 (tempvar lo)",       nes_program->data.bytes[0x01] == 0x01);
    fl_expect("Data segment at 0x02 should be 0x00 (tempvar hi)",       nes_program->data.bytes[0x02] == 0x00);

    // datavar2 = datavar
    fl_expect("Data segment at 0x03 should be 0x01 (datavar2)",         nes_program->data.bytes[0x03] == 0x01);

    zenit_nes_program_free(nes_program);
    zir_program_free(zir_program);
    zenit_context_free(&ctx);
}


void zenit_test_nes_global_vars_code(void)
{
    const char *zenit_source = 
        // CODE
        "#[NES(address: 0x2000)]"
        "var codevar : uint8 = 1;"                                    "\n"

        // CODE to DATA
        "var datavar = codevar;"                                      "\n"

        // CODE to ZP
        "#[NES(address: 0x00)]"
        "var zpvar = codevar;"                                      "\n"

        // CODE to TEMP (to DATA)
        "var tempvar = cast(codevar : uint16);"                       "\n"

        // CODE to CODE
        "#[NES(address: 0x2001)]"
        "var codevar2 = codevar;"                                       "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    struct ZirProgram *zir_program = zenit_generate_zir(&ctx);

    struct ZenitNesProgram *nes_program = zenit_nes_generate_program(zir_program);

    fl_expect("Data segment at 0x00 should be 0x00 (datavar -bss-)",    nes_program->data.bytes[0x00] == 0x00);
    fl_expect("Data segment at 0x01 should be 0x00 (tempvar -bss-)",    nes_program->data.bytes[0x01] == 0x00);
    
    // codevar = 1
    fl_expect("Startup routine at 0x00 should be 0xA9 (LDA)",           nes_program->startup.bytes[0x00] == 0xA9);
    fl_expect("Startup routine at 0x01 should be 0x00 (#$01)",          nes_program->startup.bytes[0x01] == 0x01);
    fl_expect("Startup routine at 0x02 should be 0x8D (STA)",           nes_program->startup.bytes[0x02] == 0x8D);
    fl_expect("Startup routine at 0x03 should be 0x00 ($2000 lo)",      nes_program->startup.bytes[0x03] == 0x00);
    fl_expect("Startup routine at 0x04 should be 0x20 ($2000 hi)",      nes_program->startup.bytes[0x04] == 0x20);

    // datavar = codevar
    fl_expect("Startup routine at 0x05 should be 0xAD (LDA)",           nes_program->startup.bytes[0x05] == 0xAD);
    fl_expect("Startup routine at 0x06 should be 0x00 ($2000 lo)",      nes_program->startup.bytes[0x06] == 0x00);
    fl_expect("Startup routine at 0x07 should be 0x80 ($2000 hi)",      nes_program->startup.bytes[0x07] == 0x20);
    fl_expect("Startup routine at 0x08 should be 0x8D (STA)",           nes_program->startup.bytes[0x08] == 0x8D);
    fl_expect("Startup routine at 0x09 should be 0x00 ($8000 lo)",      nes_program->startup.bytes[0x09] == 0x00);
    fl_expect("Startup routine at 0x0A should be 0x80 ($8000 hi)",      nes_program->startup.bytes[0x0A] == 0x80);

    // zpvar = codevar
    fl_expect("Startup routine at 0x0B should be 0xAD (LDA)",           nes_program->startup.bytes[0x0B] == 0xAD);
    fl_expect("Startup routine at 0x0C should be 0x00 ($2000 lo)",      nes_program->startup.bytes[0x0C] == 0x00);
    fl_expect("Startup routine at 0x0D should be 0x20 ($2000 hi)",      nes_program->startup.bytes[0x0D] == 0x20);
    fl_expect("Startup routine at 0x0E should be 0x85 (STA)",           nes_program->startup.bytes[0x0E] == 0x85);
    fl_expect("Startup routine at 0x0F should be 0x00 ($00)",           nes_program->startup.bytes[0x0F] == 0x00);

    // tempvar = cast(codevar : uint16)
    fl_expect("Startup routine at 0x10 should be 0xAD (LDA)",           nes_program->startup.bytes[0x10] == 0xAD);
    fl_expect("Startup routine at 0x11 should be 0x00 ($2000 lo)",      nes_program->startup.bytes[0x11] == 0x00);
    fl_expect("Startup routine at 0x12 should be 0x20 ($2000 hi)",      nes_program->startup.bytes[0x12] == 0x20);
    fl_expect("Startup routine at 0x13 should be 0x8D (STA)",           nes_program->startup.bytes[0x13] == 0x8D);
    fl_expect("Startup routine at 0x14 should be 0x00 ($8001 lo)",      nes_program->startup.bytes[0x14] == 0x01);
    fl_expect("Startup routine at 0x15 should be 0x80 ($8001 hi)",      nes_program->startup.bytes[0x15] == 0x80);
    fl_expect("Startup routine at 0x16 should be 0xA9 (LDA)",           nes_program->startup.bytes[0x16] == 0xA9);
    fl_expect("Startup routine at 0x17 should be 0x00 (#$00)",          nes_program->startup.bytes[0x17] == 0x00);
    fl_expect("Startup routine at 0x18 should be 0x8D (STA)",           nes_program->startup.bytes[0x18] == 0x8D);
    fl_expect("Startup routine at 0x19 should be 0x01 ($8002 lo)",      nes_program->startup.bytes[0x19] == 0x02);
    fl_expect("Startup routine at 0x1A should be 0x80 ($8002 hi)",      nes_program->startup.bytes[0x1A] == 0x80);

    // codevar2 = codevar
    fl_expect("Startup routine at 0x1B should be 0xAD (LDA)",           nes_program->startup.bytes[0x1B] == 0xAD);
    fl_expect("Startup routine at 0x1C should be 0x00 ($2000 lo)",      nes_program->startup.bytes[0x1C] == 0x00);
    fl_expect("Startup routine at 0x1D should be 0x80 ($2000 hi)",      nes_program->startup.bytes[0x1D] == 0x20);
    fl_expect("Startup routine at 0x1E should be 0x8D (STA)",           nes_program->startup.bytes[0x1E] == 0x8D);
    fl_expect("Startup routine at 0x1F should be 0x01 ($2001 lo)",      nes_program->startup.bytes[0x1F] == 0x01);
    fl_expect("Startup routine at 0x20 should be 0x20 ($2001 hi)",      nes_program->startup.bytes[0x20] == 0x20);

    zenit_nes_program_free(nes_program);
    zir_program_free(zir_program);
    zenit_context_free(&ctx);
}
