#include <stdio.h>


#include "../../Test.h"
#include "../../../src/front-end/phases/check.h"
#include "../../../src/front-end/phases/infer.h"
#include "../../../src/front-end/phases/parse.h"
#include "../../../src/front-end/phases/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/phases/zirgen.h"
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
    ;

    const char *zir_src = 
        "struct Astruct { a : uint8 }"                          "\n"

        "@a : uint8 = 1"                                        "\n"

        "@b : uint8 = 2"                                        "\n"

        "%tmp0 : [2]uint8 = [ 0, 1 ]"                           "\n"
        "@c : [2]uint8 = %tmp0"                                 "\n"

        "%tmp1 : [3]uint8 = [ 0, 1, 2 ]"                        "\n"
        "@d : [3]uint8 = %tmp1"                                 "\n"

        "@e : uint8 = @a"                                       "\n"

        "%tmp2 : &uint8 = ref @a"                               "\n"
        "@f : &uint8 = %tmp2"                                   "\n"

        "@g : &uint8 = @f"                                      "\n"

        "%tmp3 : &uint8 = ref @a"                               "\n"
        "%tmp4 : [2]&uint8 = [ %tmp3, @f ]"                     "\n"
        // FIXME: The order of the properties is unspecified because of the hashtable
        "@h : [2]&uint8 = %tmp4 ; #Attr(k2:2, k:1)"             "\n"

        "%tmp5 : [0]Astruct = [ ]"                              "\n"
        "@i : [0]Astruct = %tmp5"                               "\n"
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
    
    char *codegen = zir_program_dump(program);

    fl_expect("Generated IR must be equals to the hand-written version", flm_cstring_equals(codegen, zir_src));
    
    fl_cstring_free(codegen);
    zir_program_free(program);
}
