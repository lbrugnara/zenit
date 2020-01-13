#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/ast/ast.h"
#include "../../../src/front-end/context.h"
#include "../../../src/front-end/lexer.h"
#include "../../../src/front-end/passes/parse.h"
#include "tests.h"

void zenit_test_parser_run(const char *source, const char *ast_dump)
{
    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    bool is_valid = zenit_parse_source(&ctx);

    fl_expect("Parsing must not contain errors", is_valid);

    char *ast_str = zenit_ast_dump(ctx.ast);

    bool equals = flm_cstring_equals(ast_dump, ast_str);

    if (!equals)
        fprintf(stdout, " |-- AST: %s\n", ast_str);

    fl_cstring_free(ast_str);

    fl_vexpect(equals, "AST dump must match with the test case", ast_dump);

    zenit_context_free(&ctx);
}
