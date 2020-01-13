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

void zenit_test_nes_cast(void)
{
    const char *zenit_source = 
        "var a = [ 0x1FF, 0x2FF ];"                         "\n"
        "var b : [2]uint8 = cast(a);"                       "\n"

        "var c : [2]uint8 = cast([ 0x3FF, 0x4FF ]);"        "\n"

        "var d = [ 0x1, 0x2 ];"                             "\n"
        "var e : [2]uint16 = d;"                            "\n"

        "var f : [2]uint16 = [ 0x1, 0x2 ];"                 "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    struct ZirProgram *zir_program = zenit_generate_zir(&ctx);

    struct ZenitNesProgram *nes_program = zenit_nes_generate_program(zir_program);
    
    
    fl_expect("Data segment at 0x00 should be 0xFF (a[0] lo)",  nes_program->data.bytes[0x00] == 0xFF);
    fl_expect("Data segment at 0x01 should be 0x01 (a[0] hi)",  nes_program->data.bytes[0x01] == 0x01);
    fl_expect("Data segment at 0x02 should be 0xFF (a[1] lo)",  nes_program->data.bytes[0x02] == 0xFF);
    fl_expect("Data segment at 0x03 should be 0x02 (a[1] hi)",  nes_program->data.bytes[0x03] == 0x02);

    fl_expect("Data segment at 0x04 should be 0xFF (b[0])",     nes_program->data.bytes[0x04] == 0xFF);
    fl_expect("Data segment at 0x05 should be 0xFF (b[1])",     nes_program->data.bytes[0x05] == 0xFF);

    fl_expect("Data segment at 0x06 should be 0xFF (c[0])",     nes_program->data.bytes[0x06] == 0xFF);
    fl_expect("Data segment at 0x07 should be 0xFF (c[1])",     nes_program->data.bytes[0x07] == 0xFF);

    fl_expect("Data segment at 0x08 should be 0x01 (d[0])",     nes_program->data.bytes[0x08] == 0x01);
    fl_expect("Data segment at 0x09 should be 0x02 (d[1])",     nes_program->data.bytes[0x09] == 0x02);

    fl_expect("Data segment at 0x0A should be 0x01 (e[0] lo)",  nes_program->data.bytes[0x0A] == 0x01);
    fl_expect("Data segment at 0x0B should be 0x00 (e[0] hi)",  nes_program->data.bytes[0x0B] == 0x00);
    fl_expect("Data segment at 0x0C should be 0x02 (e[1] lo)",  nes_program->data.bytes[0x0C] == 0x02);
    fl_expect("Data segment at 0x0D should be 0x00 (e[1] hi)",  nes_program->data.bytes[0x0D] == 0x00);

    fl_expect("Data segment at 0x0E should be 0x01 (f[0] lo)",  nes_program->data.bytes[0x0E] == 0x01);
    fl_expect("Data segment at 0x0F should be 0x00 (f[0] hi)",  nes_program->data.bytes[0x0F] == 0x00);
    fl_expect("Data segment at 0x10 should be 0x02 (f[1] lo)",  nes_program->data.bytes[0x10] == 0x02);
    fl_expect("Data segment at 0x11 should be 0x00 (f[1] hi)",  nes_program->data.bytes[0x11] == 0x00);

    zenit_nes_program_free(nes_program);
    zir_program_free(zir_program);
    zenit_context_free(&ctx);
}