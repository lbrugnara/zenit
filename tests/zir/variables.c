#include <stdio.h>

#include "../Test.h"
#include "../../src/front-end/phases/check.h"
#include "../../src/front-end/phases/infer.h"
#include "../../src/front-end/phases/parse.h"
#include "../../src/front-end/phases/resolve.h"
#include "../../src/front-end/symtable.h"
#include "../../src/front-end/phases/zirgen.h"
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

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    ZirProgram *program = zenit_generate_zir(&ctx);

    fl_expect("ZIR program must compile", program != NULL);

    // At this point we can free the Zenit context, from now on, everything should work only with ZIR objects
    zenit_context_free(&ctx);
    
    char *codegen = zir_program_dump(program);

    fl_expect("Generated IR must be equals to the hand-written version", flm_cstring_equals(codegen, zir_src));
    
    fl_cstring_free(codegen);
    zir_program_free(program);
}
