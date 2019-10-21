#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/parse.h"
#include "../../../src/front-end/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void cenit_test_resolve_errors(void)
{
    const char *source = 
        "var sym_a : uint8 = 2;"                "\n"
        "var sym_a : [2]uint8 = [ 1, 2 ];"      "\n"
    ;

    CenitContext ctx = cenit_context_new("global", CENIT_SOURCE_STRING, source);

    bool valid_parse = cenit_parse_source(&ctx);
    bool valid_resolve = valid_parse ? cenit_resolve_symbols(&ctx) : false;

    fl_expect("Resolve pass must fail with 1 error", !valid_resolve && cenit_context_error_count(&ctx) == 1);
    fl_vexpect(ctx.errors[0].type == CENIT_ERROR_DUPLICATED_SYMBOL, 
        "Error must be a duplicated symbol (<source>:%u:%u): %s", ctx.errors[0].location.line, ctx.errors[0].location.col, ctx.errors[0].message);

    cenit_context_free(&ctx);
}
