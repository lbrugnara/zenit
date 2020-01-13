#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/ast/ast.h"
#include "../../../src/front-end/passes/parse.h"
#include "tests.h"

void zenit_test_parser_variable_errors(void)
{
    const char *source = 
        /* The missing semicolon after the 0 makes the parser ignore everything up to finding the semicolon */
        "/* Line 1 */ var n : uint8 = 0   ignored by synch ;"                                               "\n"
        /* Missing variable name                                                                            */
        "/* Line 2 */ var : uint8 = 1;"                                                                     "\n"
        /* The missing semicolon after the 2 makes the parser ignore everything up to finding the semicolon */
        "/* Line 3 */ var n2 : uint8 = 2  ignored by synch ;"                                               "\n"
        /* Missing type name after the colon                                                                */
        "/* Line 4 */ var n3 : = 3;"                                                                        "\n"
        /* Missing assignment operator                                                                      */
        "/* Line 5 */ var n4 4;"                                                                            "\n"
        /* Missing variable initializer                                                                     */
        "/* Line 6 */ var n5 = ;"                                                                           "\n"
        /* ULLONG_MAX + 1: Too large integral type                                                          */
        "/* Line 7 */ var n6 = 18446744073709551616;"                                                       "\n"
        /* Array without initializer                                                                        */
        "/* Line 8 */ var n7 : [2]uint8 = ;"                                                                "\n"
        /* Invalid variable name                                                                            */
        "/* Line 9 */ var $invalid = 2;"                                                                    "\n"
        /* Partial types are not allowed in struct definition                                               */
        "/* Line 10 */ struct A { arr: []uint16; }"                                                         "\n"
    ;

    const enum ZenitErrorType errors[] = {
        [0] = (enum ZenitErrorType)-1,
        /* Line */ [1] = ZENIT_ERROR_SYNTAX,
        /* Line */ [2] = ZENIT_ERROR_SYNTAX,
        /* Line */ [3] = ZENIT_ERROR_SYNTAX,
        /* Line */ [4] = ZENIT_ERROR_SYNTAX,
        /* Line */ [5] = ZENIT_ERROR_SYNTAX,
        /* Line */ [6] = ZENIT_ERROR_SYNTAX,
        /* Line */ [7] = ZENIT_ERROR_LARGE_INTEGER,
        /* Line */ [8] = ZENIT_ERROR_SYNTAX,
        /* Line */ [9] = ZENIT_ERROR_SYNTAX,
        /* Line */[10] = ZENIT_ERROR_SYNTAX,
    };

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    bool is_valid = zenit_parse_source(&ctx);

    size_t expected_errors = (sizeof(errors) / sizeof(errors[0])) - 1;
    fl_vexpect(ctx.errors != NULL && fl_array_length(ctx.errors) == expected_errors, "The context object must contain %zu errors", expected_errors);

    struct ZenitError *error = ctx.errors;

    for (size_t i=1; i <= expected_errors; i++, error++)
    {
        fl_vexpect(error->location.line == i && error->type == errors[i],
            "Expected %s error: %s at line %u:%u", errors[i] == error->type ? "syntax" : "large integer", 
            error->message, error->location.line, error->location.col);
    }

    zenit_context_free(&ctx);
}
