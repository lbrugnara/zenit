#include <stdio.h>

#include <flut/flut.h>
#include "../../src/front-end/type-check/check.h"
#include "../../src/front-end/inference/infer.h"
#include "../../src/front-end/parser/parse.h"
#include "../../src/front-end/binding/resolve.h"
#include "../../src/front-end/symtable.h"
#include "../../src/front-end/codegen/zir.h"
#include "tests.h"

void zenit_test_generate_ir_variables(void)
{
    const char *zenit_source = 
        "var a : uint8 = 1;"                "\n"
        "var b : uint8 = 2;"                "\n"
        "var c : [2]uint8 = [ 0, 1 ];"      "\n"
        "var d = [ 0, 1, 2 ];"              "\n"
        "var e = a;"                        "\n"
        "var f = &a;"                       "\n"
        "var g : &uint8 = f;"               "\n"
        "#[Attr(k:1, k2:2)]"                "\n"
        "var h = [ &a, f ];"                "\n"
        "var i : [0]Astruct = [];"          "\n"
        "struct Astruct { a: uint8; }"      "\n"
        "var b1 = true;"                    "\n"
        "var b2 = false;"                   "\n"
        "var ba: []bool = [ b1, b2 ];"      "\n"
    ;

    const char *zir_src = 
        "struct Astruct { a: uint8 }"                           "\n"

        "@a : uint8 = 1"                                        "\n"

        "@b : uint8 = 2"                                        "\n"

        "@c : [2]uint8 = [ 0, 1 ]"                              "\n"

        "@d : [3]uint8 = [ 0, 1, 2 ]"                           "\n"

        "@e : uint8 = @a"                                       "\n"

        "@f : &uint8 = ref @a"                                  "\n"

        "@g : &uint8 = @f"                                      "\n"

        // FIXME: The order of the properties is unspecified because of the hashtable
        "@h : [2]&uint8 = [ ref @a, @f ] ; #Attr(k2:2, k:1)"    "\n"

        "@i : [0]Astruct = [ ]"                                 "\n"

        "@b1 : bool = true"                                     "\n"
        "@b2 : bool = false"                                    "\n"
        "@ba : [2]bool = [ @b1, @b2 ]"                          "\n"
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

void zenit_test_generate_ir_variables_clash(void)
{
    const char *zenit_source = 
        "{ var a = 1; var x = a; }"             "\n"
        "{ var a = 2; var y = a; }"             "\n"
        "{ { { var a = 3; var z = a; } } }"     "\n"
    ;

    const char *zir_src = 
        "@a : uint8 = 1"                                        "\n"
        "@x : uint8 = @a"                                       "\n"

        "@a$l2c3 : uint8 = 2"                                  "\n"
        "@y : uint8 = @a$l2c3"                                 "\n"

        "@a$l3c7 : uint8 = 3"                                  "\n"
        "@z : uint8 = @a$l3c7"                                 "\n"
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
