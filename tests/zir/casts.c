#include <stdio.h>

#include <flut/flut.h>
#include "../../src/front-end/type-check/check.h"
#include "../../src/front-end/inference/infer.h"
#include "../../src/front-end/parser/parse.h"
#include "../../src/front-end/binding/resolve.h"
#include "../../src/front-end/symtable.h"
#include "../../src/front-end/codegen/zir.h"
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

    const char *zir_src = 
        "@a : uint16 = 1"                           "\n"

        "%tmp0 : uint8 = cast(511, uint8)"          "\n"
        "@b : uint8 = %tmp0"                        "\n"
        
        "%tmp1 : uint8 = cast(512, uint8)"          "\n"
        "@c : uint8 = %tmp1"                        "\n"

        "%tmp2 : uint8 = cast(513, uint8)"          "\n"
        "@d : uint8 = %tmp2"                        "\n"

        "%tmp3 : uint16 = cast(ref @d, uint16)"      "\n"
        "@e : uint16 = %tmp3"                       "\n"
    ;

    ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    flut_expect_compat("Parsing should not contain errors", zenit_parse_source(&ctx));
    flut_expect_compat("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    flut_expect_compat("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    flut_expect_compat("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    ZirProgram *program = zenit_generate_zir(&ctx);

    flut_expect_compat("ZIR program must compile", program != NULL);

    // At this point we can free the Zenit context, from now on, everything should work only with ZIR objects
    zenit_context_free(&ctx);
    
    char *codegen = zir_program_dump(program);

    flut_expect_compat("Generated IR must be equals to the hand-written version", flm_cstring_equals(codegen, zir_src));
    
    fl_cstring_free(codegen);

    zir_program_free(program);
}
