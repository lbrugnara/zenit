#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/passes/parse.h"
#include "../../../src/front-end/passes/utils.h"
#include "../../../src/front-end/program.h"
#include "../../../src/front-end/passes/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/types/system.h"
#include "tests.h"

void zenit_test_resolve_struct_declaration(void)
{
    const char *source = 
        "var v = 1;"                                    "\n"
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
        "#[Attr(k: v)]"                                 "\n"
        "struct H { h: uint16; }"                       "\n"
    ;

    const char *program_dump = 
        "(program "
            "(scope global "
                "(symbol v uint8) "
                "(scope struct A (symbol a uint8)) "
                "(scope struct B (symbol b uint16)) "
                "(scope struct C (symbol c &uint8)) "
                "(scope struct D (symbol d [2]uint8)) "
                "(scope struct E (symbol e2 uint8) (symbol e &[2]uint8)) "
                "(scope struct F (symbol f [10]E)) "
                "(scope struct G"
                    " (symbol a A)"
                    " (symbol b B)"
                    " (symbol c C)"
                    " (symbol d D)"
                    " (symbol e E)"
                    " (symbol f F)"
                ") "
                "(scope struct H (symbol h uint16))"
            ")"
        ")"
    ;

    zenit_test_resolve_run(source, program_dump);
}
