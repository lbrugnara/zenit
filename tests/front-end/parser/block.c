

#include <flut/flut.h>
#include "../../../src/front-end/ast/ast.h"
#include "../../../src/front-end/phases/parse.h"
#include "tests.h"

void zenit_test_parser_blocks(void)
{
    const char *source = 
        "var a = 1;"                "\n"
        "var b = 2;"                "\n"
        "{"                         "\n"
        "   var c = true;"          "\n"
        "   var d = false;"         "\n"
        "   {"                      "\n"
        "       var e = { a: 1};"   "\n"
        "       var f = { b: 2};"   "\n"
        "   }"                      "\n"
        "}"                         "\n"
    ;

    const char *ast_dump = 
        "(ast"
        " (var a (uint8 1))"
        " (var b (uint8 2)) "
            "("
                "(var c (bool true)) "
                "(var d (bool false)) "
                "("
                    "(var e (struct (a (uint8 1)))) "
                    "(var f (struct (b (uint8 2))))"
                ")"
            ")"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}
