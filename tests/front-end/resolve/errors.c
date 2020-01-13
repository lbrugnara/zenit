#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/passes/parse.h"
#include "../../../src/front-end/passes/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void zenit_test_resolve_errors(void)
{
    const char *source = 
        "var sym_a : uint8 = 2;"                "\n"
        "var sym_a : [2]uint8 = [ 1, 2 ];"      "\n"
        "var sym_b = sym_not_defined;"          "\n"
        "var sym_c = [ sym_not_defined2 ];"     "\n"
        "var sym_d = &unknown;"                 "\n"
        "#[Attr(key:missing_symbol)]"           "\n"
        "var sym_e = 1;"                        "\n"
        "struct A { a: uint8; a: uint16; }"     "\n"
        "struct B { b: uint8; }"                "\n"
        "struct B { b: uint8; }"                "\n"
    ;

    struct TestCase {
        enum ZenitErrorType error;
        const char *message;
    } tests[] = {
        { ZENIT_ERROR_DUPLICATED_SYMBOL,    "sym_a already exists"                 },
        { ZENIT_ERROR_MISSING_SYMBOL,       "sym_not_defined does not exist"       },
        { ZENIT_ERROR_MISSING_SYMBOL,       "sym_not_defined2 does not exist"      },
        { ZENIT_ERROR_MISSING_SYMBOL,       "unknown does not exist"               },
        { ZENIT_ERROR_MISSING_SYMBOL,       "missing_symbol does not exist"        },
        { ZENIT_ERROR_DUPLICATED_SYMBOL,    "struct A already has an 'a' field"    },
        { ZENIT_ERROR_DUPLICATED_SYMBOL,    "struct B already exists"              },
    };

    size_t error_count = sizeof(tests) / sizeof(tests[0]);

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    bool valid_parse = zenit_parse_source(&ctx);
    bool valid_resolve = valid_parse ? zenit_resolve_symbols(&ctx) : false;

    fl_vexpect(!valid_resolve && zenit_context_error_count(&ctx) == error_count, "Resolve pass must fail with %zu errors", error_count);

    for (size_t i=0; i < error_count; i++)
        fl_vexpect(ctx.errors[i].type == tests[i].error, 
            "L%u:%u: %s (%s)",
            ctx.errors[i].location.line, ctx.errors[i].location.col, ctx.errors[i].message, tests[i].message);

    zenit_context_free(&ctx);
}
