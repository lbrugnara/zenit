#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/check.h"
#include "../../../src/front-end/infer.h"
#include "../../../src/front-end/parse.h"
#include "../../../src/front-end/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

struct ExpectedError {
    CenitErrorType type;
    const char *message;
} expected_errors[] = {
    { CENIT_ERROR_MISSING_SYMBOL, "'custom' type is not defined (<source>:%u:%u: %s)" },
    { CENIT_ERROR_TYPE_MISSMATCH, "Cannot assign [2]uint8 to uint8 (<source>:%u:%u: %s)" },
    { CENIT_ERROR_TYPE_MISSMATCH, "Cannot assign uint8 to [2]uint8 (<source>:%u:%u: %s)" },
};

void cenit_test_check_types_errors(void)
{
    const char *source = 
        "var sym_a : [0]custom = [];"               "\n"
        "var sym_b : uint8 = [ 1, 2 ];"             "\n"
        "var sym_c : [2]uint8 = 0;"                 "\n"
    ;

    CenitContext ctx = cenit_context_new("global", CENIT_SOURCE_STRING, source);

    fl_expect("Parsing should not contain errors", cenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", cenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", cenit_infer_types(&ctx));

    size_t error_count = sizeof(expected_errors) / sizeof(expected_errors[0]);
    fl_vexpect(!cenit_check_types(&ctx) && cenit_context_error_count(&ctx) == error_count, "Type check pass must fail with %zu error(s)", error_count);
    
    for (size_t i=0; i < error_count; i++)
    {
        fl_vexpect(ctx.errors[i].type == expected_errors[i].type, 
            expected_errors[i].message, 
            ctx.errors[i].location.line, ctx.errors[i].location.col, ctx.errors[i].message);
    }

    cenit_context_free(&ctx);
}
