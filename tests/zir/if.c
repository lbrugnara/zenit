#include <stdio.h>

#include "../Test.h"
#include "../../src/front-end/phases/check.h"
#include "../../src/front-end/phases/infer.h"
#include "../../src/front-end/phases/parse.h"
#include "../../src/front-end/phases/resolve.h"
#include "../../src/front-end/symtable.h"
#include "../../src/front-end/phases/zirgen.h"
#include "tests.h"

void zenit_test_generate_ir_if(void)
{
    const char *zenit_source = 
        "if (true) {}"                                                                  "\n"
        
        "if (true) { var a = 1; }"                                                      "\n"
        
        "if (true) { var b = 1; } else { var c = 2; }"                                  "\n"
        
        "if (true) { var d = 1; }"                                                      "\n"
        "else if (false) { var e = 3;  }"                                               "\n"
        "else { var f = 2; }"                                                           "\n"

        "if (true) { if (true) { if (false) {} } }"                                     "\n"

        "if (true) { var x = 1; if (true) { if (false) {} var y = 2; } }"               "\n"

        "if (true) { var j = 1; if (true) {} else { if (false) {} var k = 2; } }"       "\n"
    ;

    const char *zir_src = 
        "if_false true jump 1"                          "\n" // if (true) {}

        "if_false true jump 2"                          "\n" // if (true)
        "@a : uint8 = 1"                                "\n" //     var a = 1;

        "if_false true jump 3"                          "\n" // if (true)
        "@b : uint8 = 1"                                "\n" //     var b = 1;
        "jump 2"                                        "\n" // 
        "@c : uint8 = 2"                                "\n" // else var c = 2;

        "if_false true jump 3"                          "\n" // if (true)
        "@d : uint8 = 1"                                "\n" //     var d = 1;
        "jump 5"                                        "\n" // 
        "if_false false jump 3"                         "\n" // else if (false)
        "@e : uint8 = 3"                                "\n" //     var e = 3;
        "jump 2"                                        "\n" //
        "@f : uint8 = 2"                                "\n" // else var f = 2;

        "if_false true jump 3"                          "\n" // if (true)
        "if_false true jump 2"                          "\n" //     if (true)
        "if_false false jump 1"                         "\n" //         if (false) {}

        "if_false true jump 5"                          "\n" // if (true)
        "@x : uint8 = 1"                                "\n" //     var x = 1;
        "if_false true jump 3"                          "\n" //     if (true) 
        "if_false false jump 1"                         "\n" //         if (false) {}
        "@y : uint8 = 2"                                "\n" //         var y = 2;

        "if_false true jump 6"                          "\n" // if (true)
        "@j : uint8 = 1"                                "\n" //     var x = 1;
        "if_false true jump 2"                          "\n" //     if (true) {}
        "jump 3"                                        "\n" //     else
        "if_false false jump 1"                         "\n" //         if (false) {}
        "@k : uint8 = 2"                                "\n" //         var y = 2;
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
