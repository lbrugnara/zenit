#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/ast/ast.h"
#include "../../../src/front-end/passes/parse.h"
#include "tests.h"

void zenit_test_parser_structs(void)
{
    const char *source = 
        "struct A { a: uint8; }"            "\n"
        "struct B { b: uint16; }"           "\n"
        "struct C { c: &uint8; }"           "\n"
        "struct D { d: [2]uint8; }"         "\n"
    ;

    const char *ast_dump = 
        "(ast"
        " (struct A (field a (type uint8)))"
        " (struct B (field b (type uint16)))"
        " (struct C (field c (type &uint8)))"
        " (struct D (field d (type [2]uint8)))"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}
