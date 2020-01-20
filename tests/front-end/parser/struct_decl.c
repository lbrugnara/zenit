#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/ast/ast.h"
#include "../../../src/front-end/phases/parse.h"
#include "tests.h"

void zenit_test_parser_struct_decl(void)
{
    const char *source = 
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

    const char *ast_dump = 
        "(ast"
        " (struct-decl A (field a (type uint8)))"
        " (struct-decl B (field b (type uint16)))"
        " (struct-decl C (field c (type &uint8)))"
        " (struct-decl D (field d (type [2]uint8)))"
        " (struct-decl E (field e (type &[2]uint8)) (field e2 (type uint8)))"
        " (struct-decl F (field f (type [10]E)))"
        " (struct-decl G (field a (type A)) (field b (type B)) (field c (type C)) (field d (type D)) (field e (type E)) (field f (type F)))"
        " (struct-decl H (field h (type uint16)) (attr Attr (prop k (id v))))"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}
