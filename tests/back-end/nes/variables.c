#include <stdio.h>


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

        "var p1 = { x: 1, y: 2 };"                                      "\n"
        "var p2 = { x: 0x1FF, y: 0x2FF };"                              "\n"
        "var p3 = { a: p2, b: p1 };"                                    "\n"
        "var parr = [ { a: 1 }, { a: 2 }, { a: 3 }, { a: 0x1FF } ];"    "\n"

        "var b1 = true;"                                                "\n"
        "var b2 = false;"                                               "\n"
        "var ba = [ true, false ];"                                     "\n"
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
    fl_expect("Data segment at 0x21 should be 0x01 (p1.x)",             nes_program->data.bytes[0x21] == 0x01);
    fl_expect("Data segment at 0x22 should be 0x02 (p1.y)",             nes_program->data.bytes[0x22] == 0x02);
    fl_expect("Data segment at 0x23 should be 0xFF (p2.x lo)",          nes_program->data.bytes[0x23] == 0xFF);
    fl_expect("Data segment at 0x24 should be 0x01 (p2.x hi)",          nes_program->data.bytes[0x24] == 0x01);
    fl_expect("Data segment at 0x25 should be 0xFF (p2.y lo)",          nes_program->data.bytes[0x25] == 0xFF);
    fl_expect("Data segment at 0x26 should be 0x02 (p2.y hi)",          nes_program->data.bytes[0x26] == 0x02);
    fl_expect("Data segment at 0x27 should be 0xFF (p3.a.x lo)",        nes_program->data.bytes[0x27] == 0xFF);
    fl_expect("Data segment at 0x28 should be 0x01 (p3.a.x hi)",        nes_program->data.bytes[0x28] == 0x01);
    fl_expect("Data segment at 0x29 should be 0xFF (p3.a.y lo)",        nes_program->data.bytes[0x29] == 0xFF);
    fl_expect("Data segment at 0x2A should be 0x02 (p3.a.y hi)",        nes_program->data.bytes[0x2A] == 0x02);
    fl_expect("Data segment at 0x2B should be 0x01 (p3.b.x)",           nes_program->data.bytes[0x2B] == 0x01);
    fl_expect("Data segment at 0x2C should be 0x02 (p3.b.y)",           nes_program->data.bytes[0x2C] == 0x02);
    fl_expect("Data segment at 0x2D should be 0x01 (parr[0].a lo)",     nes_program->data.bytes[0x2D] == 0x01);
    fl_expect("Data segment at 0x2E should be 0x00 (parr[0].a hi)",     nes_program->data.bytes[0x2E] == 0x00);
    fl_expect("Data segment at 0x2F should be 0x02 (parr[1].a lo)",     nes_program->data.bytes[0x2F] == 0x02);
    fl_expect("Data segment at 0x30 should be 0x00 (parr[1].a hi)",     nes_program->data.bytes[0x30] == 0x00);
    fl_expect("Data segment at 0x31 should be 0x03 (parr[2].a lo)",     nes_program->data.bytes[0x31] == 0x03);
    fl_expect("Data segment at 0x32 should be 0x00 (parr[2].a hi)",     nes_program->data.bytes[0x32] == 0x00);
    fl_expect("Data segment at 0x33 should be 0xFF (parr[3].a lo)",     nes_program->data.bytes[0x33] == 0xFF);
    fl_expect("Data segment at 0x34 should be 0x01 (parr[3].a hi)",     nes_program->data.bytes[0x34] == 0x01);
    fl_expect("Data segment at 0x35 should be 0x01 (b1)",               nes_program->data.bytes[0x35] == 0x01);
    fl_expect("Data segment at 0x36 should be 0x00 (b2)",               nes_program->data.bytes[0x36] == 0x00);
    fl_expect("Data segment at 0x37 should be 0x01 (ba[0])",            nes_program->data.bytes[0x37] == 0x01);
    fl_expect("Data segment at 0x38 should be 0x00 (ba[1])",            nes_program->data.bytes[0x38] == 0x00);

    zenit_nes_program_free(nes_program);
    zir_program_free(zir_program);
    zenit_context_free(&ctx);
}

void zenit_test_nes_global_vars_array(void)
{
    const char *zenit_source = 
        "var a = 0x1ff;"                                                "\n"
        "var arr = [ 0, 1, 2 ];"                                        "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    struct ZirProgram *zir_program = zenit_generate_zir(&ctx);

    struct ZenitNesProgram *nes_program = zenit_nes_generate_program(zir_program);
    
    fl_expect("Data segment at 0x00 should be 0xff (a lo)",             nes_program->data.bytes[0x00] == 0xFF);
    fl_expect("Data segment at 0x01 should be 0x01 (a hi)",             nes_program->data.bytes[0x01] == 0x01);
    fl_expect("Data segment at 0x02 should be 0x01 (arr[0])",           nes_program->data.bytes[0x02] == 0x00);
    fl_expect("Data segment at 0x03 should be 0x01 (arr[1])",           nes_program->data.bytes[0x03] == 0x01);
    fl_expect("Data segment at 0x04 should be 0x02 (arr[2])",           nes_program->data.bytes[0x04] == 0x02);

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

        // ZP allocation of structs
        "#[NES(address: 0x02)]"
        "var p1 = { x: 1, y: 2 };"                                  "\n"
        
        // ZP allocation of array of structs
        "#[NES(address: 0x04)]"
        "var parr = [ { a: 1 }, { a: 2 }, { a: 3 }, { a: 0x1FF } ];""\n"
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

    // ZP allocation of structs
    // Allocate x = 1
    fl_expect("Startup routine at 0x1C should be 0xA9 (LDA)",           nes_program->startup.bytes[0x1C] == 0xA9);
    fl_expect("Startup routine at 0x1D should be 0x01 (#$01)",          nes_program->startup.bytes[0x1D] == 0x01);
    fl_expect("Startup routine at 0x1E should be 0x85 (STA)",           nes_program->startup.bytes[0x1E] == 0x85);
    fl_expect("Startup routine at 0x1F should be 0x01 ($02)",           nes_program->startup.bytes[0x1F] == 0x02);
    // Allocate y = 2
    fl_expect("Startup routine at 0x20 should be 0xA9 (LDA)",           nes_program->startup.bytes[0x20] == 0xA9);
    fl_expect("Startup routine at 0x21 should be 0x01 (#$02)",          nes_program->startup.bytes[0x21] == 0x02);
    fl_expect("Startup routine at 0x22 should be 0x85 (STA)",           nes_program->startup.bytes[0x22] == 0x85);
    fl_expect("Startup routine at 0x23 should be 0x03 ($03)",           nes_program->startup.bytes[0x23] == 0x03);

    // ZP allocation of array of structs
    // Allocate parr[0] = { a: 1 }
    fl_expect("Startup routine at 0x24 should be 0xA9 (LDA)",           nes_program->startup.bytes[0x24] == 0xA9);
    fl_expect("Startup routine at 0x25 should be 0x01 (#$01)",          nes_program->startup.bytes[0x25] == 0x01);
    fl_expect("Startup routine at 0x26 should be 0x85 (STA)",           nes_program->startup.bytes[0x26] == 0x85);
    fl_expect("Startup routine at 0x27 should be 0x04 ($04)",           nes_program->startup.bytes[0x27] == 0x04);
    fl_expect("Startup routine at 0x28 should be 0xA2 (LDX)",           nes_program->startup.bytes[0x28] == 0xA2);
    fl_expect("Startup routine at 0x29 should be 0x00 (#$00)",          nes_program->startup.bytes[0x29] == 0x00);
    fl_expect("Startup routine at 0x2A should be 0x86 (STX)",           nes_program->startup.bytes[0x2A] == 0x86);
    fl_expect("Startup routine at 0x2B should be 0x05 ($05)",           nes_program->startup.bytes[0x2B] == 0x05);
    // Allocate parr[1] = { a: 2 }
    fl_expect("Startup routine at 0x2C should be 0xA9 (LDA)",           nes_program->startup.bytes[0x2C] == 0xA9);
    fl_expect("Startup routine at 0x2D should be 0x02 (#$02)",          nes_program->startup.bytes[0x2D] == 0x02);
    fl_expect("Startup routine at 0x2E should be 0x85 (STA)",           nes_program->startup.bytes[0x2E] == 0x85);
    fl_expect("Startup routine at 0x2F should be 0x06 ($06)",           nes_program->startup.bytes[0x2F] == 0x06);
    fl_expect("Startup routine at 0x30 should be 0xA2 (LDX)",           nes_program->startup.bytes[0x30] == 0xA2);
    fl_expect("Startup routine at 0x31 should be 0x00 (#$00)",          nes_program->startup.bytes[0x31] == 0x00);
    fl_expect("Startup routine at 0x32 should be 0x86 (STX)",           nes_program->startup.bytes[0x32] == 0x86);
    fl_expect("Startup routine at 0x33 should be 0x07 ($07)",           nes_program->startup.bytes[0x33] == 0x07);
    // Allocate parr[2] = { a: 3 }
    fl_expect("Startup routine at 0x34 should be 0xA9 (LDA)",           nes_program->startup.bytes[0x34] == 0xA9);
    fl_expect("Startup routine at 0x35 should be 0x03 (#$03)",          nes_program->startup.bytes[0x35] == 0x03);
    fl_expect("Startup routine at 0x36 should be 0x85 (STA)",           nes_program->startup.bytes[0x36] == 0x85);
    fl_expect("Startup routine at 0x37 should be 0x08 ($08)",           nes_program->startup.bytes[0x37] == 0x08);
    fl_expect("Startup routine at 0x38 should be 0xA2 (LDX)",           nes_program->startup.bytes[0x38] == 0xA2);
    fl_expect("Startup routine at 0x39 should be 0x00 (#$00)",          nes_program->startup.bytes[0x39] == 0x00);
    fl_expect("Startup routine at 0x3A should be 0x86 (STX)",           nes_program->startup.bytes[0x3A] == 0x86);
    fl_expect("Startup routine at 0x3B should be 0x09 ($09)",           nes_program->startup.bytes[0x3B] == 0x09);
    // Allocate parr[3] = { a: 0x1FF }
    fl_expect("Startup routine at 0x3C should be 0xA9 (LDA)",           nes_program->startup.bytes[0x3C] == 0xA9);
    fl_expect("Startup routine at 0x3D should be 0xFF (#$FF)",          nes_program->startup.bytes[0x3D] == 0xFF);
    fl_expect("Startup routine at 0x3E should be 0x85 (STA)",           nes_program->startup.bytes[0x3E] == 0x85);
    fl_expect("Startup routine at 0x3F should be 0x0A ($0A)",           nes_program->startup.bytes[0x3F] == 0x0A);
    fl_expect("Startup routine at 0x40 should be 0xA2 (LDX)",           nes_program->startup.bytes[0x40] == 0xA2);
    fl_expect("Startup routine at 0x41 should be 0x01 (#$10)",          nes_program->startup.bytes[0x41] == 0x01);
    fl_expect("Startup routine at 0x42 should be 0x86 (STX)",           nes_program->startup.bytes[0x42] == 0x86);
    fl_expect("Startup routine at 0x43 should be 0x0B ($0B)",           nes_program->startup.bytes[0x43] == 0x0B);

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
        "var codevar : uint8 = 1;"                                      "\n"

        // CODE to DATA
        "var datavar = codevar;"                                        "\n"

        // CODE to ZP
        "#[NES(address: 0x00)]"
        "var zpvar = codevar;"                                          "\n"

        // CODE to TEMP (to DATA)
        "var tempvar = cast(codevar : uint16);"                         "\n"

        // CODE to CODE
        "#[NES(address: 0x2001)]"
        "var codevar2 = codevar;"                                       "\n"

        // CODE allocation of structs
        "#[NES(address: 0x2002)]"
        "var p1 = { x: 1, y: 2 };"                                      "\n"
        
        // CODE allocation of array of structs
        "#[NES(address: 0x2004)]"
        "var parr = [ { a: 1 }, { a: 2 }, { a: 3 }, { a: 0x1FF } ];"    "\n"
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
    fl_expect("Startup routine at 0x14 should be 0x01 ($8001 lo)",      nes_program->startup.bytes[0x14] == 0x01);
    fl_expect("Startup routine at 0x15 should be 0x80 ($8001 hi)",      nes_program->startup.bytes[0x15] == 0x80);
    fl_expect("Startup routine at 0x16 should be 0xA9 (LDA)",           nes_program->startup.bytes[0x16] == 0xA9);
    fl_expect("Startup routine at 0x17 should be 0x00 (#$00)",          nes_program->startup.bytes[0x17] == 0x00);
    fl_expect("Startup routine at 0x18 should be 0x8D (STA)",           nes_program->startup.bytes[0x18] == 0x8D);
    fl_expect("Startup routine at 0x19 should be 0x02 ($8002 lo)",      nes_program->startup.bytes[0x19] == 0x02);
    fl_expect("Startup routine at 0x1A should be 0x80 ($8002 hi)",      nes_program->startup.bytes[0x1A] == 0x80);

    // codevar2 = codevar
    fl_expect("Startup routine at 0x1B should be 0xAD (LDA)",           nes_program->startup.bytes[0x1B] == 0xAD);
    fl_expect("Startup routine at 0x1C should be 0x00 ($2000 lo)",      nes_program->startup.bytes[0x1C] == 0x00);
    fl_expect("Startup routine at 0x1D should be 0x80 ($2000 hi)",      nes_program->startup.bytes[0x1D] == 0x20);
    fl_expect("Startup routine at 0x1E should be 0x8D (STA)",           nes_program->startup.bytes[0x1E] == 0x8D);
    fl_expect("Startup routine at 0x1F should be 0x01 ($2001 lo)",      nes_program->startup.bytes[0x1F] == 0x01);
    fl_expect("Startup routine at 0x20 should be 0x20 ($2001 hi)",      nes_program->startup.bytes[0x20] == 0x20);

    // CODE allocation of structs
    // Allocate x = 1
    fl_expect("Startup routine at 0x21 should be 0xA9 (LDA)",           nes_program->startup.bytes[0x21] == 0xA9);
    fl_expect("Startup routine at 0x22 should be 0x01 (#$01)",          nes_program->startup.bytes[0x22] == 0x01);
    fl_expect("Startup routine at 0x23 should be 0x8D (STA)",           nes_program->startup.bytes[0x23] == 0x8D);
    fl_expect("Startup routine at 0x24 should be 0x02 ($2002 lo)",      nes_program->startup.bytes[0x24] == 0x02);
    fl_expect("Startup routine at 0x25 should be 0x20 ($2002 hi)",      nes_program->startup.bytes[0x25] == 0x20);
    // Allocate y = 2
    fl_expect("Startup routine at 0x26 should be 0xA9 (LDA)",           nes_program->startup.bytes[0x26] == 0xA9);
    fl_expect("Startup routine at 0x27 should be 0x02 (#$02)",          nes_program->startup.bytes[0x27] == 0x02);
    fl_expect("Startup routine at 0x28 should be 0x8D (STA)",           nes_program->startup.bytes[0x28] == 0x8D);
    fl_expect("Startup routine at 0x29 should be 0x03 ($2003 lo)",      nes_program->startup.bytes[0x29] == 0x03);
    fl_expect("Startup routine at 0x2A should be 0x20 ($2003 hi)",      nes_program->startup.bytes[0x2A] == 0x20);

    // CODE allocation of array of structs
    // Allocate parr[0] = { a: 1 }
    fl_expect("Startup routine at 0x2B should be 0xA9 (LDA)",           nes_program->startup.bytes[0x2B] == 0xA9);
    fl_expect("Startup routine at 0x2C should be 0x01 (#$01)",          nes_program->startup.bytes[0x2C] == 0x01);
    fl_expect("Startup routine at 0x2D should be 0x8D (STA)",           nes_program->startup.bytes[0x2D] == 0x8D);
    fl_expect("Startup routine at 0x2E should be 0x04 ($2004 lo)",      nes_program->startup.bytes[0x2E] == 0x04);
    fl_expect("Startup routine at 0x2F should be 0x20 ($2004 hi)",      nes_program->startup.bytes[0x2F] == 0x20);
    fl_expect("Startup routine at 0x30 should be 0xA2 (LDX)",           nes_program->startup.bytes[0x30] == 0xA2);
    fl_expect("Startup routine at 0x31 should be 0x00 (#$00)",          nes_program->startup.bytes[0x31] == 0x00);
    fl_expect("Startup routine at 0x32 should be 0x8E (STX)",           nes_program->startup.bytes[0x32] == 0x8E);
    fl_expect("Startup routine at 0x33 should be 0x05 ($2005 lo)",      nes_program->startup.bytes[0x33] == 0x05);
    fl_expect("Startup routine at 0x34 should be 0x20 ($2005 hi)",      nes_program->startup.bytes[0x34] == 0x20);
    // Allocate parr[1] = { a: 2 }
    fl_expect("Startup routine at 0x35 should be 0xA9 (LDA)",           nes_program->startup.bytes[0x35] == 0xA9);
    fl_expect("Startup routine at 0x36 should be 0x02 (#$02)",          nes_program->startup.bytes[0x36] == 0x02);
    fl_expect("Startup routine at 0x37 should be 0x8D (STA)",           nes_program->startup.bytes[0x37] == 0x8D);
    fl_expect("Startup routine at 0x38 should be 0x06 ($2006 lo)",      nes_program->startup.bytes[0x38] == 0x06);
    fl_expect("Startup routine at 0x39 should be 0x20 ($2006 hi)",      nes_program->startup.bytes[0x39] == 0x20);
    fl_expect("Startup routine at 0x3A should be 0xA2 (LDX)",           nes_program->startup.bytes[0x3A] == 0xA2);
    fl_expect("Startup routine at 0x3B should be 0x00 (#$00)",          nes_program->startup.bytes[0x3B] == 0x00);
    fl_expect("Startup routine at 0x3C should be 0x8E (STX)",           nes_program->startup.bytes[0x3C] == 0x8E);
    fl_expect("Startup routine at 0x3D should be 0x07 ($2007 lo)",      nes_program->startup.bytes[0x3D] == 0x07);
    fl_expect("Startup routine at 0x3E should be 0x20 ($2007 hi)",      nes_program->startup.bytes[0x3E] == 0x20);
    // Allocate parr[2] = { a: 3 }
    fl_expect("Startup routine at 0x3F should be 0xA9 (LDA)",           nes_program->startup.bytes[0x3F] == 0xA9);
    fl_expect("Startup routine at 0x40 should be 0x03 (#$03)",          nes_program->startup.bytes[0x40] == 0x03);
    fl_expect("Startup routine at 0x41 should be 0x8D (STA)",           nes_program->startup.bytes[0x41] == 0x8D);
    fl_expect("Startup routine at 0x42 should be 0x08 ($2008 lo)",      nes_program->startup.bytes[0x42] == 0x08);
    fl_expect("Startup routine at 0x43 should be 0x20 ($2008 hi)",      nes_program->startup.bytes[0x43] == 0x20);
    fl_expect("Startup routine at 0x44 should be 0xA2 (LDX)",           nes_program->startup.bytes[0x44] == 0xA2);
    fl_expect("Startup routine at 0x45 should be 0x00 (#$00)",          nes_program->startup.bytes[0x45] == 0x00);
    fl_expect("Startup routine at 0x46 should be 0x8E (STX)",           nes_program->startup.bytes[0x46] == 0x8E);
    fl_expect("Startup routine at 0x47 should be 0x09 ($2009 lo)",      nes_program->startup.bytes[0x47] == 0x09);
    fl_expect("Startup routine at 0x48 should be 0x20 ($2009 hi)",      nes_program->startup.bytes[0x48] == 0x20);
    // Allocate parr[3] = { a: 0x1FF }
    fl_expect("Startup routine at 0x49 should be 0xA9 (LDA)",           nes_program->startup.bytes[0x49] == 0xA9);
    fl_expect("Startup routine at 0x4A should be 0xFF (#$FF)",          nes_program->startup.bytes[0x4A] == 0xFF);
    fl_expect("Startup routine at 0x4B should be 0x8D (STA)",           nes_program->startup.bytes[0x4B] == 0x8D);
    fl_expect("Startup routine at 0x4C should be 0x0A ($200A lo)",      nes_program->startup.bytes[0x4C] == 0x0A);
    fl_expect("Startup routine at 0x4D should be 0x20 ($200A hi)",      nes_program->startup.bytes[0x4D] == 0x20);
    fl_expect("Startup routine at 0x4E should be 0xA2 (LDX)",           nes_program->startup.bytes[0x4E] == 0xA2);
    fl_expect("Startup routine at 0x4F should be 0x01 (#$01)",          nes_program->startup.bytes[0x4F] == 0x01);
    fl_expect("Startup routine at 0x50 should be 0x8E (STX)",           nes_program->startup.bytes[0x50] == 0x8E);
    fl_expect("Startup routine at 0x51 should be 0x0B ($200B lo)",      nes_program->startup.bytes[0x51] == 0x0B);
    fl_expect("Startup routine at 0x52 should be 0x20 ($200B hi)",      nes_program->startup.bytes[0x52] == 0x20);

    zenit_nes_program_free(nes_program);
    zir_program_free(zir_program);
    zenit_context_free(&ctx);
}
