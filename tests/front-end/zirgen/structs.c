#include <stdio.h>
#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/passes/check.h"
#include "../../../src/front-end/passes/infer.h"
#include "../../../src/front-end/passes/parse.h"
#include "../../../src/front-end/passes/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/passes/zirgen.h"
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
        "struct A { a : uint8 }"                        "\n"
        "struct B { b : uint16 }"                       "\n"
        "struct C { c : &uint8 }"                       "\n"
        "struct D { d : [2]uint8 }"                     "\n"
        "struct E { e2 : uint8, e : &[2]uint8 }"        "\n"
        "struct F { f : [10]E }"                        "\n"
        "struct G { "
            "a : A, " 
            "b : B, " 
            "c : C, " 
            "d : D, " 
            "e : E, " 
            "f : F " 
        "}"                                             "\n"
        "struct H { h : uint16 }"                       "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    struct ZirProgram *program = zenit_generate_zir(&ctx);

    fl_expect("ZIR program must compile", program != NULL);

    // At this point we can free the Zenit context, from now on, everything should work only with ZIR objects
    zenit_context_free(&ctx);
    
    char *codegen = zir_program_dump(program, fl_cstring_new(0));

    fl_expect("Generated IR must be equals to the hand-written version", flm_cstring_equals(codegen, zir_src));
    
    fl_cstring_free(codegen);
    zir_program_free(program);
}
