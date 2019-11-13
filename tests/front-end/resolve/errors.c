#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/parse.h"
#include "../../../src/front-end/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void zenit_test_resolve_errors(void)
{
    const char *source = 
        "var sym_a : uint8 = 2;"                "\n"
        "var sym_a : [2]uint8 = [ 1, 2 ];"      "\n"
        "var sym_b = sym_not_defined;"          "\n"
        "var sym_c = [ sym_not_defined2 ];"     "\n"
        "var sym_d = &unknown;"                 "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    bool valid_parse = zenit_parse_source(&ctx);
    bool valid_resolve = valid_parse ? zenit_resolve_symbols(&ctx) : false;

    fl_expect("Resolve pass must fail with 4 errors", !valid_resolve && zenit_context_error_count(&ctx) == 4);
    
    fl_vexpect(ctx.errors[0].type == ZENIT_ERROR_DUPLICATED_SYMBOL, 
        "Error must be a duplicated symbol (<source>:%u:%u): %s", ctx.errors[0].location.line, ctx.errors[0].location.col, ctx.errors[0].message);

    fl_vexpect(ctx.errors[1].type == ZENIT_ERROR_MISSING_SYMBOL, 
        "Error must be a missing symbol (<source>:%u:%u): %s", ctx.errors[1].location.line, ctx.errors[1].location.col, ctx.errors[1].message);

    fl_vexpect(ctx.errors[2].type == ZENIT_ERROR_MISSING_SYMBOL, 
        "Error must be a missing symbol (<source>:%u:%u): %s", ctx.errors[2].location.line, ctx.errors[2].location.col, ctx.errors[2].message);

    fl_vexpect(ctx.errors[3].type == ZENIT_ERROR_MISSING_SYMBOL, 
        "Error must be a missing symbol (<source>:%u:%u): %s", ctx.errors[3].location.line, ctx.errors[3].location.col, ctx.errors[3].message);

    zenit_context_free(&ctx);
}
