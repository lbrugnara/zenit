#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/passes/infer.h"
#include "../../../src/front-end/passes/parse.h"
#include "../../../src/front-end/passes/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void zenit_test_infer_errors(void)
{
    const char *source = 
        "var sym_a : [0]custom = [];"               "\n"
        "var sym_b : [2]uint8 = [ 1, sym_a ];"      "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    bool valid_parse = zenit_parse_source(&ctx);
    bool valid_resolve = valid_parse ? zenit_resolve_symbols(&ctx) : false;
    bool valid_infer = valid_parse ? zenit_infer_types(&ctx) : false;

    fl_expect("Infer pass must fail with 1 error", !valid_infer && zenit_context_error_count(&ctx) == 1);
    
    fl_vexpect(ctx.errors[0].type == ZENIT_ERROR_INFERENCE, 
        "Cannot unify types (<source>:%u:%u): %s", ctx.errors[0].location.line, ctx.errors[0].location.col, ctx.errors[0].message);

    zenit_context_free(&ctx);
}
