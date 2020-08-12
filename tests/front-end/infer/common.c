

#include <flut/flut.h>
#include "../../../src/front-end/inference/infer.h"
#include "../../../src/front-end/phases/parse.h"
#include "../../../src/front-end/program.h"
#include "../../../src/front-end/types/context.h"
#include "../../../src/front-end/phases/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void zenit_test_infer_run(const char *source, const char *test_case, bool verbose)
{
    ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    bool is_valid = zenit_parse_source(&ctx);

    flut_expect_compat("Parsing must not contain errors", is_valid);

    is_valid = zenit_resolve_symbols(&ctx);

    flut_expect_compat("Resolve pass must not contain errors", is_valid);

    is_valid = zenit_infer_types(&ctx);

    flut_expect_compat("Inference pass must not contain errors", is_valid);

    char *program_dump = zenit_program_dump(ctx.program, verbose);

    bool equals = flm_cstring_equals(test_case, program_dump);

    if (!equals)
        fprintf(stdout, " |-- Program: %s\n", program_dump);

    fl_cstring_free(program_dump);

    flut_vexpect_compat(equals, "Program dump must match with the test case", test_case);

    zenit_context_free(&ctx);
}
