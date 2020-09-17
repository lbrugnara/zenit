

#include <flut/flut.h>
#include "../../../src/front-end/ast/ast.h"
#include "../../../src/front-end/parser/parse.h"
#include "tests.h"

void zenit_test_parser_attributes_variables(void)
{
    const char *source = 
        "#[a1]"                                 "\n"
        "#[a2()]"                               "\n"
        "#[a3(k: v)]"                           "\n"
        "#[a4(k: v, k2: v2)]"                   "\n"
        "var variable = 0;"                     "\n"

        "#[NES(address: 0x2000)]"               "\n"
        "var PPUCTRL : uint8 = 1;"              "\n"
        "#[NES(segment: zp, address: 0)]"       "\n"
        "var spaceship : uint8 = 1;"            "\n"
    ;

    const char *ast_dump = 
        "(ast"
        " (var variable (uint8 0) (attr a1) (attr a2) (attr a3 (prop k (id v))) (attr a4 (prop k2 (id v2)) (prop k (id v))))"
        " (var PPUCTRL (type uint8) (uint8 1) (attr NES (prop address (uint16 8192))))"
        " (var spaceship (type uint8) (uint8 1) (attr NES (prop segment (id zp)) (prop address (uint8 0))))"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}
