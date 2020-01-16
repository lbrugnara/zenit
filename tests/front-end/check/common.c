#include <stdio.h>
#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/passes/check.h"
#include "../../../src/front-end/passes/infer.h"
#include "../../../src/front-end/passes/parse.h"
#include "../../../src/front-end/passes/resolve.h"
#include "../../../src/front-end/program.h"
#include "tests.h"

void zenit_test_check_types(const char *source, const char *cases[][2], size_t length)
{
    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    for (size_t i=0; i < length; i++)
    {
        const char **test = cases[i];

        fl_vexpect(zenit_program_has_symbol(ctx.program, test[0]), 
            "Symbol '%s' must exist in the program", test[0]);

        struct ZenitSymbol *sym = zenit_program_get_symbol(ctx.program, test[0]);

        fl_vexpect(flm_cstring_equals(test[1], zenit_type_to_string(sym->typeinfo.type)), 
            "Type of '%s' must be '%s'", test[0], test[1]);
    }
    
    zenit_context_free(&ctx);
}


void zenit_test_check_type_errors(const char *source, struct ExpectedError *cases, size_t length)
{
    size_t error_count = length;
    size_t run_errors = 0;
    bool run_success = false;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    
    run_success = zenit_infer_types(&ctx);
    run_errors = zenit_context_error_count(&ctx);

    if (run_errors != error_count)
        zenit_context_print_errors(&ctx);

    fl_expect("Type inference pass should not contain errors", run_success);

    run_success = zenit_check_types(&ctx);
    run_errors = zenit_context_error_count(&ctx);

    if (run_errors != error_count)
        zenit_context_print_errors(&ctx);

    fl_vexpect(!run_success && run_errors == error_count, "Type check pass must fail with %zu error(s) (errors: %zu)", error_count, run_errors);

    size_t i=0;
    struct FlListNode *tmp = fl_list_head(ctx.errors);
    while (tmp != NULL)
    {
        struct ZenitError *error = (struct ZenitError*) tmp->value;

        fl_vexpect(error->type == cases[i].type, 
            cases[i].message, 
            error->location.line, error->location.col, error->message);

        tmp = tmp->next;
        i++;
    }
    

    zenit_context_free(&ctx);
}
