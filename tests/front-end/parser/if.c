

#include <flut/flut.h>
#include "../../../src/front-end/ast/ast.h"
#include "../../../src/front-end/parser/parse.h"
#include "tests.h"

void zenit_test_parser_if_statements(void)
{
    const char *source = 
        "if (true) {}"                                  "\n"
        "if (true) {} else {}"                          "\n"
        "if (true) {} else if (true) {}"                "\n"
        "if (true) {} else if (true) {} else {}"        "\n"
        "if (false) { var a = 1; }"                     "\n"
        "var b = true;"                                 "\n"
        "if (b) { var x = 1; } else { var x = 2; }"     "\n"
        "if (b) {"                                      "\n"
            "var x = true;"                             "\n"
            "var y = false;"                            "\n"
        "} else if (false) {"                           "\n"
            "var x  = 0x1FF;"                           "\n"
            "var y  = 0x2FF;"                           "\n"
        "} else {"                                      "\n"
            "var x  = { a: 1 };"                        "\n"
            "var y  = { b: 2 };"                        "\n"
        "}"                                             "\n"
        "if (b) {"                                      "\n"
            "if (b) {"                                  "\n"
                "var a = 1;"                            "\n"
            "} else {"                                  "\n"
                "var a = 2;"                            "\n"
            "}"                                         "\n"
        "}"                                             "\n"
    ;

    const char *ast_dump = 
        "(ast"
            " (if (bool true) ())"
            " (if (bool true) () ())"
            " (if (bool true) () (if (bool true) ()))"
            " (if (bool true) () (if (bool true) () ()))"
            " (if (bool false) ((var a (uint8 1))))"
            " (var b (bool true))"
            " (if (id b) ((var x (uint8 1))) ((var x (uint8 2))))"
            " (if (id b)"
                " ((var x (bool true)) (var y (bool false)))"
                " (if (bool false) "
                    "((var x (uint16 511)) (var y (uint16 767))) "
                    "((var x (struct (a (uint8 1)))) (var y (struct (b (uint8 2)))))))"
            " (if (id b)"
                " ((if (id b) "
                    "((var a (uint8 1))) "
                    "((var a (uint8 2))))))"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}
