

#include "../../Test.h"
#include "../../../src/front-end/ast/ast.h"
#include "../../../src/front-end/context.h"
#include "../../../src/front-end/lexer.h"
#include "../../../src/front-end/phases/parse.h"
#include "tests.h"

void zenit_test_parser_literal_integer(void)
{
    const char *source = 
            "/* uint8 */ 1;"                    "\n"
            "/* uint8 */ 2;"                    "\n"
            "/* uint8 */ 3;"                    "\n"
            "/* uint8 */ 255;"                  "\n"
            "/* uint16 */ 8192;"                "\n"
            //"/* uint64 */ 1844674407370955161;  \n"
    ;

    const char *ast_dump =
        "(ast"
            " (uint8 1)"
            " (uint8 2)"
            " (uint8 3)"
            " (uint8 255)"
            " (uint16 8192)"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}

void zenit_test_parser_literal_boolean(void)
{
    const char *source = 
        "true;"     "\n"
        "false;"     "\n"
    ;

    const char *ast_dump =
        "(ast"
            " (bool true)"
            " (bool false)"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}

void zenit_test_parser_literal_array_literal(void)
{
    const char *source = 
            /* [3]uint8 */
            "[ 1, 2, 3 ];"      "\n"
            "[ ];"              "\n"
    ;

    const char *ast_dump = 
        "(ast"
        " (array (uint8 1) (uint8 2) (uint8 3))"
        " (array)"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}

void zenit_test_parser_literal_integer_error(void)
{
    const char *source = 
        /* ULLONG_MAX + 1: Too large integral type*/
        "18446744073709551616;          \n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_valid = zenit_parse_source(&ctx);

    size_t expected_errors = 1;
    fl_vexpect(!is_valid && ctx.errors != NULL && fl_list_length(ctx.errors) == expected_errors, "The context object must contain %zu error(s)", expected_errors);

    size_t i=1;
    struct FlListNode *tmp = fl_list_head(ctx.errors);
    while (tmp != NULL)
    {
        struct ZenitError *error = (struct ZenitError*) tmp->value;

        fl_vexpect(error->location.line == 1 && error->type == ZENIT_ERROR_LARGE_INTEGER, 
            "Expected semantic error: %s at line %u:%u", error->message, error->location.line, error->location.col);

        tmp = tmp->next;
        i++;
    }

    zenit_context_free(&ctx);
}
