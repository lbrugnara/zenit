

#include <flut/flut.h>
#include "../../../src/front-end/parser/parse.h"
#include "../../../src/front-end/program.h"
#include "../../../src/front-end/binding/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/types/context.h"
#include "tests.h"

void zenit_test_resolve_run(const char *source, const char *test_case, bool verbose)
{
    ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    bool is_valid = zenit_parse_source(&ctx);

    flut_expect_compat("Parsing must not contain errors", is_valid);

    is_valid = zenit_resolve_symbols(&ctx);

    flut_expect_compat("Resolve pass must not contain errors", is_valid);

    char *program_dump = zenit_program_dump(ctx.program, verbose);

    bool equals = flm_cstring_equals(test_case, program_dump);

    if (!equals)
        fprintf(stdout, " |-- Program: %s\n", program_dump);

    fl_cstring_free(program_dump);

    flut_vexpect_compat(equals, "Program dump must match with the test case", test_case);

    zenit_context_free(&ctx);
}

void zenit_test_resolve_errors(const char *source, struct ResolveTestCase *tests, size_t tests_count)
{
    ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    bool valid_parse = zenit_parse_source(&ctx);
    bool valid_resolve = valid_parse ? zenit_resolve_symbols(&ctx) : false;

    flut_vexpect_compat(!valid_resolve && zenit_context_error_count(&ctx) == tests_count, "Resolve pass must fail with %zu errors", tests_count);

    size_t i=0;
    struct FlListNode *tmp = fl_list_head(ctx.errors);
    while (tmp != NULL)
    {
        ZenitError *error = (ZenitError*) tmp->value;

        flut_vexpect_compat(error->type == tests[i].error, 
            "L%u:%u: %s (%s)",
            error->location.line, error->location.col, error->message, tests[i].message);

        tmp = tmp->next;
        i++;
    }

    zenit_context_free(&ctx);
}
