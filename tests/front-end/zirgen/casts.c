#include <stdio.h>
#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/check.h"
#include "../../../src/front-end/infer.h"
#include "../../../src/front-end/parse.h"
#include "../../../src/front-end/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/zirgen.h"
#include "tests.h"

void zenit_test_generate_ir_casts(void)
{
    const char *zenit_source = 
        // Implicit (up) cast
        "var a : uint16 = 1;"                       "\n"

        // Explicit (down) cast (with truncation)
        "var b : uint8 = cast(0x1FF : uint8);"      "\n"

        // Explicit (down) cast (the cast's type is inferred)
        "var c : uint8 = cast(0x200);"              "\n"

        // Explicit (down) cast (d's type is inferred)
        "var d = cast(0x201 : uint8);"              "\n"

        // Explicit (down) cast (the cast's type is inferred)
        "var e : uint16 = cast(&d);"                "\n"
    ;

    const char *zenit_ir_src = 
        "@a : uint16 = 1"                           "\n"

        "%tmp0 : uint8 = cast(511, uint8)"          "\n"
        "@b : uint8 = %tmp0"                        "\n"
        
        "%tmp1 : uint8 = cast(512, uint8)"          "\n"
        "@c : uint8 = %tmp1"                        "\n"

        "%tmp2 : uint8 = cast(513, uint8)"          "\n"
        "@d : uint8 = %tmp2"                        "\n"

        "%tmp3 : uint16 = cast(ref @d, uint16)"     "\n"
        "@e : uint16 = %tmp3"                       "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    struct ZenitIrProgram *program = zenit_generate_zir(&ctx);
    
    char *codegen = zenit_ir_program_dump(program, fl_cstring_new(0));

    fl_expect("Generated IR must be equals to the hand-written version", flm_cstring_equals(codegen, zenit_ir_src));
    
    fl_cstring_free(codegen);

    zenit_ir_program_free(program);
    zenit_context_free(&ctx);
}
