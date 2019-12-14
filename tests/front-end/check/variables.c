#include <stdio.h>
#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/passes/check.h"
#include "../../../src/front-end/passes/infer.h"
#include "../../../src/front-end/passes/parse.h"
#include "../../../src/front-end/passes/resolve.h"
#include "../../../src/front-end/program.h"
#include "tests.h"

struct Test {
    const char *sym_name;
    const char *sym_type;
} tests[] = {
    { "a",          "[3][2]uint8"   },
    { "a_ref",      "&[3][2]uint8"  },
    { "a_ref_ref",  "&&[3][2]uint8"  },
};

void zenit_test_check_types_variables(void)
{
    const char *zenit_source = 
        "var a = [ [ 3, 4 ], [ 5, 6 ], [ 7, 8 ] ];"         "\n"
        "var a_ref = &a;"                                   "\n"
        "var a_ref_ref = &a_ref;"                           "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    for (size_t i=0; i < sizeof(tests) / sizeof(tests[0]); i++)
    {
        struct Test *test = tests + i;

        fl_vexpect(zenit_program_has_symbol(ctx.program, test->sym_name), 
            "Symbol '%s' must exist in the program", test->sym_name);

        struct ZenitSymbol *sym = zenit_program_get_symbol(ctx.program, test->sym_name);

        fl_vexpect(flm_cstring_equals(test->sym_type, zenit_type_to_string(sym->typeinfo)), 
            "Type of '%s' must be '%s'", test->sym_name, test->sym_type);
    }
    
    zenit_context_free(&ctx);
}