#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/phases/parse.h"
#include "../../../src/front-end/phases/resolve.h"
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
        "var b1 = B { b: 1, c: 2 };"            "\n"
        "struct Point { x: uint8; y: uint8; }"  "\n"
        "var p = Point { x: 1 };"               "\n"
        "var p2 : Point = { y: 1 };"            "\n" // Here '{ y: 1 }' is resolved as an unnamed struct, it will fail later in the type check
    ;

    struct TestCase {
        enum ZenitErrorType error;
        const char *message;
    } tests[] = {
        { ZENIT_ERROR_DUPLICATED_SYMBOL,    "sym_a already exists"                      },
        { ZENIT_ERROR_MISSING_SYMBOL,       "sym_not_defined does not exist"            },
        { ZENIT_ERROR_MISSING_SYMBOL,       "sym_not_defined2 does not exist"           },
        { ZENIT_ERROR_MISSING_SYMBOL,       "unknown does not exist"                    },
        { ZENIT_ERROR_MISSING_SYMBOL,       "missing_symbol does not exist"             },
        { ZENIT_ERROR_DUPLICATED_SYMBOL,    "struct A already has an 'a' field"         },
        { ZENIT_ERROR_DUPLICATED_SYMBOL,    "struct B already exists"                   },
        { ZENIT_ERROR_UNKNOWN_MEMBER,       "struct B does not contain a 'c' field"     },
        { ZENIT_ERROR_UNINITIALIZED_MEMBER, "Member 'y' must be initialized"            },
    };

    size_t error_count = sizeof(tests) / sizeof(tests[0]);

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    bool valid_parse = zenit_parse_source(&ctx);
    bool valid_resolve = valid_parse ? zenit_resolve_symbols(&ctx) : false;

    fl_vexpect(!valid_resolve && zenit_context_error_count(&ctx) == error_count, "Resolve pass must fail with %zu errors", error_count);

    size_t i=0;
    struct FlListNode *tmp = fl_list_head(ctx.errors);
    while (tmp != NULL)
    {
        struct ZenitError *error = (struct ZenitError*) tmp->value;

        fl_vexpect(error->type == tests[i].error, 
            "L%u:%u: %s (%s)",
            error->location.line, error->location.col, error->message, tests[i].message);

        tmp = tmp->next;
        i++;
    }

    zenit_context_free(&ctx);
}
