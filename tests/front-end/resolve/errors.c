

#include "../../Test.h"
#include "../../../src/front-end/phases/parse.h"
#include "../../../src/front-end/phases/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void zenit_test_resolve_variable_errors(void)
{
    const char *source = 
        "var sym_a : uint8 = 2;"                "\n"
        "var sym_a : [2]uint8 = [ 1, 2 ];"      "\n"
        "var sym_b = sym_not_defined;"          "\n"
        "var sym_c = [ sym_not_defined2 ];"     "\n"
        "var sym_d = &unknown;"                 "\n"
        "#[Attr(key:missing_symbol)]"           "\n"
        "var sym_e = 1;"                        "\n"
    ;

    struct ResolveTestCase tests[] = {
        { ZENIT_ERROR_DUPLICATED_SYMBOL,    "sym_a already exists"                      },
        { ZENIT_ERROR_MISSING_SYMBOL,       "sym_not_defined does not exist"            },
        { ZENIT_ERROR_MISSING_SYMBOL,       "sym_not_defined2 does not exist"           },
        { ZENIT_ERROR_MISSING_SYMBOL,       "unknown does not exist"                    },
        { ZENIT_ERROR_MISSING_SYMBOL,       "missing_symbol does not exist"             },
    };

    size_t error_count = sizeof(tests) / sizeof(tests[0]);

    zenit_test_resolve_errors(source, tests, error_count);
}

void zenit_test_resolve_struct_errors(void)
{
    const char *source = 
        "struct A { a: uint8; a: uint16; }"     "\n"
        "struct B { b: uint8; }"                "\n"
        "struct B { b: uint8; }"                "\n"
        "var b1 = B { b: 1, c: 2 };"            "\n"
        "struct Point { x: uint8; y: uint8; }"  "\n"
        "var p = Point { x: 1 };"               "\n"
        "var p2 : Point = { y: 1 };"            "\n" // Here '{ y: 1 }' is resolved as an unnamed struct, it will fail later in the type check
        "struct C { x: [2]MissingType; }"       "\n"
        "var w = W { x: 1, y: 2 };"             "\n"
    ;

    struct ResolveTestCase tests[] = {
        { ZENIT_ERROR_DUPLICATED_SYMBOL,    "struct A already has an 'a' field"         },
        { ZENIT_ERROR_DUPLICATED_SYMBOL,    "struct B already exists"                   },
        { ZENIT_ERROR_UNKNOWN_MEMBER,       "struct B does not contain a 'c' field"     },
        { ZENIT_ERROR_UNINITIALIZED_MEMBER, "Member 'y' must be initialized"            },
        { ZENIT_ERROR_MISSING_SYMBOL,       "MissingType does not exist"                },
        { ZENIT_ERROR_MISSING_SYMBOL,       "W does not exist"                          },
    };

    size_t error_count = sizeof(tests) / sizeof(tests[0]);

    zenit_test_resolve_errors(source, tests, error_count);
}

