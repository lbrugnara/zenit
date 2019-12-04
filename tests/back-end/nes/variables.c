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

void zenit_test_nes_global_variables(void)
{
    const char *zenit_source = 
        "#[NES(address:2)]"                 "\n"
        "var a : uint8 = 1;"                "\n"
        "var b : uint8 = 2;"                "\n"
        "#[NES(address: 0x2000)]"           "\n"
        "var ppuctrl : uint16 = 0;"         "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    struct ZenitIrProgram *program = zenit_generate_zir(&ctx);

    struct ZenitNesProgram *rom = zenit_nes_generate(program);
    

    zenit_nes_program_free(rom);
    zenit_ir_program_free(program);
    zenit_context_free(&ctx);
}
