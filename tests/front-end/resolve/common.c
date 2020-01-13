#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/passes/parse.h"
#include "../../../src/front-end/passes/utils.h"
#include "../../../src/front-end/program.h"
#include "../../../src/front-end/passes/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/types/system.h"
#include "tests.h"

void zenit_test_resolve_run(const char *source, const char *test_case)
{
    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    bool is_valid = zenit_parse_source(&ctx);

    fl_expect("Parsing must not contain errors", is_valid);

    is_valid = zenit_resolve_symbols(&ctx);

    fl_expect("Resolve pass must not contain errors", is_valid);

    char *program_dump = zenit_program_dump(ctx.program);

    bool equals = flm_cstring_equals(test_case, program_dump);

    if (!equals)
        fprintf(stdout, " |-- Program: %s\n", program_dump);

    fl_cstring_free(program_dump);

    fl_vexpect(equals, "Program dump must match with the test case", test_case);

    zenit_context_free(&ctx);
}
