#include <stdio.h>


#include <flut/flut.h>
#include "../../../src/front-end/phases/check.h"
#include "../../../src/front-end/inference/infer.h"
#include "../../../src/front-end/phases/parse.h"
#include "../../../src/front-end/phases/resolve.h"
#include "../../../src/front-end/program.h"
#include "tests.h"

void zenit_test_check_types(const char *source, const char *cases[][2], size_t length)
{
    ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    flut_expect_compat("Parsing should not contain errors", zenit_parse_source(&ctx));
    flut_expect_compat("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    flut_expect_compat("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    flut_expect_compat("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    for (size_t i=0; i < length; i++)
    {
        const char **test = cases[i];

        flut_vexpect_compat(zenit_program_has_symbol(ctx.program, test[0]), 
            "Symbol '%s' must exist in the program", test[0]);

        ZenitSymbol *sym = zenit_program_get_symbol(ctx.program, test[0]);

        flut_vexpect_compat(flm_cstring_equals(test[1], zenit_type_to_string(sym->type)), 
            "Type of '%s' must be '%s'", test[0], test[1]);
    }
    
    zenit_context_free(&ctx);
}


void zenit_test_check_type_errors(const char *source, struct ExpectedError *cases, size_t length)
{
    size_t error_count = length;
    size_t run_errors = 0;
    bool run_success = false;

    ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    flut_expect_compat("Parsing should not contain errors", zenit_parse_source(&ctx));
    flut_expect_compat("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    
    run_success = zenit_infer_types(&ctx);
    run_errors = zenit_context_error_count(&ctx);

    if (run_errors != error_count)
        zenit_context_print_errors(&ctx);

    flut_expect_compat("Type inference pass should not contain errors", run_success);

    run_success = zenit_check_types(&ctx);
    run_errors = zenit_context_error_count(&ctx);

    if (run_errors != error_count)
        zenit_context_print_errors(&ctx);

    flut_vexpect_compat(!run_success && run_errors == error_count, "Type check pass must fail with %zu error(s) (errors: %zu)", error_count, run_errors);

    size_t i=0;
    struct FlListNode *tmp = fl_list_head(ctx.errors);
    while (tmp != NULL)
    {
        ZenitError *error = (ZenitError*) tmp->value;

        flut_vexpect_compat(error->type == cases[i].type, 
            cases[i].message, 
            error->location.line, error->location.col, error->message);

        tmp = tmp->next;
        i++;
    }
    

    zenit_context_free(&ctx);
}
