#include <stdio.h>

#include <flut/flut.h>
#include "../../src/front-end/phases/check.h"
#include "../../src/front-end/inference/infer.h"
#include "../../src/front-end/parser/parse.h"
#include "../../src/front-end/phases/resolve.h"
#include "../../src/front-end/symtable.h"
#include "../../src/front-end/phases/zirgen.h"
#include "tests.h"

void zenit_test_generate_ir_struct_decl(void)
{
    const char *zenit_source = 
        "struct A { a: uint8; }"                        "\n"
        "struct B { b: uint16; }"                       "\n"
        "struct C { c: &uint8; }"                       "\n"
        "struct D { d: [2]uint8; }"                     "\n"
        "struct E { e: &[2]uint8; e2: uint8; }"         "\n"
        "struct F { f: [10]E; }"                        "\n"
        "struct G {"                                    "\n"
        "   a: A;"                                      "\n"
        "   b: B;"                                      "\n"
        "   c: C;"                                      "\n"
        "   d: D;"                                      "\n"
        "   e: E;"                                      "\n"
        "   f: F;"                                      "\n"
        "}"                                             "\n"
        "struct H { h: uint16; }"                       "\n"
    ;

    const char *zir_src = 
        "struct A { a: uint8 }"                         "\n"
        "struct B { b: uint16 }"                        "\n"
        "struct C { c: &uint8 }"                        "\n"
        "struct D { d: [2]uint8 }"                      "\n"
        "struct E { e: &[2]uint8, e2: uint8 }"          "\n"
        "struct F { f: [10]E }"                         "\n"
        "struct G { "
            "a: A, " 
            "b: B, " 
            "c: C, " 
            "d: D, " 
            "e: E, " 
            "f: F " 
        "}"                                             "\n"
        "struct H { h: uint16 }"                        "\n"
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

void zenit_test_generate_ir_struct(void)
{
    const char *zenit_source = 
        "struct A { a: uint8; }"                                "\n"
        "var a = A { a: 1 };"                                   "\n"

        "struct B { a: uint8; b: uint16; }"                     "\n"
        "var b : B = { a: 1, b: 0x1FF };"                       "\n"

        "struct C { a: A; b: B; }"                              "\n"
        "var c = C { a: { a: 1 }, b: { a: 2, b: 300 } };"       "\n"

        "var array : []A = [ { a: 1 }, { a: 2 }, A { a: 3 } ];" "\n"
    ;

    const char *zir_src = 
        "struct A { a: uint8 }"                                 "\n"
        "struct B { a: uint8, b: uint16 }"                      "\n"
        "struct C { a: A, b: B }"                               "\n"
        "@a : A = { a: 1 }"                                     "\n"
        "@b : B = { a: 1, b: 511 }"                             "\n"
        "@c : C = { a: { a: 1 }, b: { a: 2, b: 300 } }"         "\n"
        "@array : [3]A = [ { a: 1 }, { a: 2 }, { a: 3 } ]"      "\n"
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
