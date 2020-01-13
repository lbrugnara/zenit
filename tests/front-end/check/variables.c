#include <stdio.h>
#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/passes/check.h"
#include "../../../src/front-end/passes/infer.h"
#include "../../../src/front-end/passes/parse.h"
#include "../../../src/front-end/passes/resolve.h"
#include "../../../src/front-end/program.h"
#include "tests.h"

void zenit_test_check_types_variables(void)
{
    const char *zenit_source = 
        "var a = [ [ 3, 4 ], [ 5, 6 ], [ 7, 8 ] ];"         "\n"
        "var a_ref = &a;"                                   "\n"
        "var a_ref_ref = &a_ref;"                           "\n"
        "var sym_t : [1]uint8 = [ cast(0x1FF) ];"           "\n"
        
        "var l = [ 1, 2, 3];"                               "\n"
        "var m : &[3]uint16 = &l;"                          "\n"
        "var n : &[3]uint8 = &l;"                           "\n"

        "var o = [ 0x1FF, 0x2FF, 0x3FF];"                   "\n"
        "var p : &[3]uint8 = cast(&o);"                     "\n"
        "var q : &[3]uint16 = &o;"                          "\n"

        "var r = [ 0x1, 0x2 ];"                             "\n"
        "var s : [2]uint16 = r;"                            "\n"

        "var t : []&[][]uint8 = [ &a, a_ref ];"             "\n"

        "var u : [0]Astruct = [];"                          "\n"
        "var v : [0]Astruct = u;"                           "\n"
        "var w = u;"                                        "\n"
        "struct Astruct { a: uint8; }"                      "\n" // No need to forward declare the struct
    ;

    const char *tests[][2] = {
        { "a",          "[3][2]uint8"       },
        { "a_ref",      "&[3][2]uint8"      },
        { "a_ref_ref",  "&&[3][2]uint8"     },
        { "sym_t",      "[1]uint8"          },

        { "l",          "[3]uint8"          },
        { "m",          "&[3]uint16"        },
        { "n",          "&[3]uint8"         },

        { "o",          "[3]uint16"         },
        { "p",          "&[3]uint8"         },
        { "q",          "&[3]uint16"        },

        { "r",          "[2]uint8"          },
        { "s",          "[2]uint16"         },

        { "t",          "[2]&[3][2]uint8"   },

        { "u",          "[0]Astruct"        },
        { "v",          "[0]Astruct"        },
        { "w",          "[0]Astruct"        },
    };

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, zenit_source);

    fl_expect("Parsing should not contain errors", zenit_parse_source(&ctx));
    fl_expect("Symbol resolving pass should not contain errors", zenit_resolve_symbols(&ctx));
    fl_expect("Type inference pass should not contain errors", zenit_infer_types(&ctx));
    fl_expect("Type check pass should not contain errors", zenit_check_types(&ctx));
    
    for (size_t i=0; i < sizeof(tests) / sizeof(tests[0]); i++)
    {
        const char **test = tests[i];

        fl_vexpect(zenit_program_has_symbol(ctx.program, test[0]), 
            "Symbol '%s' must exist in the program", test[0]);

        struct ZenitSymbol *sym = zenit_program_get_symbol(ctx.program, test[0]);

        fl_vexpect(flm_cstring_equals(test[1], zenit_type_to_string(sym->typeinfo)), 
            "Type of '%s' must be '%s'", test[0], test[1]);
    }
    
    zenit_context_free(&ctx);
}
