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

void zenit_test_generate_zir_variables(void)
{
    const char *zenit_source = 
        "var a : uint8 = 1;"                "\n"
        "var b : uint8 = 2;"                "\n"
        "var c : [2]uint8 = [ 0, 1 ];"      "\n"
        "var d = [ 0, 1, 2 ];"              "\n"
        "var e = a;"                        "\n"
        "var f = &a;"                       "\n"
        "var g : &uint8 = f;"               "\n"
        "var h = [ &a, f ];"                "\n"
    ;

    const char *zir_src = 
        "@a : uint8 = 1"                      "\n"
        "@b : uint8 = 2"                      "\n"
        "@c : [2]uint8 = [ 0, 1 ]"            "\n"
        "@d : [3]uint8 = [ 0, 1, 2 ]"         "\n"
        "@e : uint8 = @a"                     "\n"
        "@f : &uint8 = ref @a"                "\n"
        "@g : &uint8 = @f"                    "\n"
        "@h : [2]&uint8 = [ ref @a, @f ]"     "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    struct ZirProgram *program = zenit_generate_zir(&ctx);
    
    char *codegen = zir_program_dump(program, fl_cstring_new(0));

    fl_expect("Generated IR must be equals to the hand-written version", flm_cstring_equals(codegen, zir_src));
    
    fl_cstring_free(codegen);

    zir_program_free(program);
    zenit_context_free(&ctx);
}
