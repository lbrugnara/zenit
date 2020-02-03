

#include "../../Test.h"
#include "../../../src/front-end/ast/ast.h"
#include "../../../src/front-end/context.h"
#include "../../../src/front-end/lexer.h"
#include "../../../src/front-end/phases/parse.h"
#include "tests.h"

void zenit_test_parser_run(const char *source, const char *test_case)
{
    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    bool is_valid = zenit_parse_source(&ctx);

    fl_expect("Parsing must not contain errors", is_valid);

    char *ast_dump = zenit_ast_dump(ctx.ast);

    bool equals = flm_cstring_equals(test_case, ast_dump);

    if (!equals)
        fprintf(stdout, " |-- AST: %s\n", ast_dump);

    fl_cstring_free(ast_dump);

    fl_vexpect(equals, "AST dump must match with the test case", test_case);

    zenit_context_free(&ctx);
}
