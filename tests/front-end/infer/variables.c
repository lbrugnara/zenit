

#include "../../Test.h"
#include "../../../src/front-end/phases/infer.h"
#include "../../../src/front-end/phases/parse.h"
#include "../../../src/front-end/program.h"
#include "../../../src/front-end/types/context.h"
#include "../../../src/front-end/phases/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void zenit_test_infer_variable_uint(void)
{
    const char *source = 
        "var sym_a = 2;"                        "\n"
        "var sym_b = 1;"                        "\n"
        "var sym_c = sym_b;"                    "\n"
        "var sym_d = 0x1FF;"                    "\n"
        "var sym_e = cast(sym_d : uint8);"      "\n"
        "var sym_f = cast(&sym_d : uint8);"     "\n"
    ;

    const char *tests[][2] = { 
        { "sym_a", "uint8" },
        { "sym_b", "uint8" },
        { "sym_c", "uint8" },
        { "sym_d", "uint16" },
        { "sym_e", "uint8" },
        { "sym_f", "uint8" },
    };

    const size_t count = sizeof(tests) / sizeof(tests[0]);

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_parsing_valid = zenit_parse_source(&ctx);
    bool is_resolve_valid = is_parsing_valid && zenit_resolve_symbols(&ctx);
    bool is_inference_valid = is_resolve_valid && zenit_infer_types(&ctx);
    
    fl_expect("Parsing, symbol resolving, and type inference phases should not contain errors", is_inference_valid);

    for (size_t i=0; i < count; i++)
    {
        const char **test = tests[i];

        fl_vexpect(zenit_program_has_symbol(ctx.program, test[0]), "Symbol table must contain symbol \"%s\"", test[0]);

        struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx.program, test[0]);

        fl_vexpect(flm_cstring_equals(zenit_type_to_string(symbol->type), test[1]), "Symbol %s's type must be %s", test[0], test[1]);
    }

    zenit_context_free(&ctx);
}

void zenit_test_infer_variable_reference(void)
{
    const char *source = 
        "var sym_a = 1;"                                "\n"
        "var sym_b = 0x1FF;"                            "\n"

        "var sym_c = &sym_a;"                           "\n"
        "var sym_d = &sym_b;"                           "\n"
        "var sym_e = &sym_d;"                           "\n"
        "var sym_f = cast(&sym_b : &uint8);"            "\n"
        "var sym_g = cast(&sym_a : &uint16);"           "\n" // Not valid (it doesn't pass the type check, but the inference for sym_g should still work)

        "var sym_h = { x: 1, y: 2, z: [ 1, 0x1FF ] };"  "\n"
        "var sym_i = &sym_h;"                           "\n"

        "struct Point { x: uint8; y: uint8; }"          "\n"
        "var sym_j = { x: 1, y: 2};"                    "\n"
        "var p = cast(sym_j : Point);"                  "\n"
        "var p2 : Point = cast(sym_j);"                 "\n"
        "var p3 : Point = sym_j;"                       "\n"
    ;
    
    const char *tests[][2] = { 
        { "sym_a",  "uint8"                                     },
        { "sym_b",  "uint16"                                    },
        { "sym_c",  "&uint8"                                    },
        { "sym_d",  "&uint16"                                   },
        { "sym_e",  "&&uint16"                                  },
        { "sym_f",  "&uint8"                                    },
        { "sym_g",  "&uint16"                                   },
        { "sym_h",  "{ x: uint8, y: uint8, z: [2]uint16 }"      },
        { "sym_i",  "&{ x: uint8, y: uint8, z: [2]uint16 }"     },
        { "sym_j",  "{ x: uint8, y: uint8 }"                    },
        { "p",      "Point"                                     },
        { "p2",     "Point"                                     },
        { "p3",     "Point"                                     },
    };

    const size_t count = sizeof(tests) / sizeof(tests[0]);

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_parsing_valid = zenit_parse_source(&ctx);
    bool is_resolve_valid = is_parsing_valid && zenit_resolve_symbols(&ctx);
    bool is_inference_valid = is_resolve_valid && zenit_infer_types(&ctx);
    
    fl_expect("Parsing, symbol resolving, and type inference phases should not contain errors", is_inference_valid);

    for (size_t i=0; i < count; i++)
    {
        const char **test = tests[i];

        fl_vexpect(zenit_program_has_symbol(ctx.program, test[0]), "Symbol table must contain symbol \"%s\"", test[0]);

        struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx.program, test[0]);

        fl_vexpect(flm_cstring_equals(zenit_type_to_string(symbol->type), test[1]), "Symbol %s's type must be %s", test[0], test[1]);
    }

    zenit_context_free(&ctx);
}

void zenit_test_infer_variable_array(void)
{
    const char *source = 
        "var sym_a : uint8 = 2;"                                        "\n"
        "var sym_b : uint16 = 1;"                                       "\n"
        "var sym_c : customType = 0;"                                   "\n"
        "var sym_d = sym_c;"                                            "\n"
        "var sym_e : &uint16 = &sym_b;"                                  "\n"
        "var sym_f = &sym_e;"                                           "\n"
        "var sym_g : [2]uint8 = [ 1, 2 ];"                              "\n"
        "var sym_h = [ 1, 2, 3 ];"                                      "\n"
        "var sym_i : [0]customType = [];"                               "\n"
        "var sym_j : [1]uint16 = [ sym_b ];"                            "\n"
        "var sym_k = [ 8192 ];"                                         "\n"
        "var sym_l = [ 1, 8192 ];"                                      "\n"
        "var sym_m = [ 1, 8192, 3];"                                    "\n"
        "var sym_n = [ cast(&sym_a : uint16) ];"                        "\n"
        "var sym_o : [2]uint8 = [ cast(&sym_a), cast(&sym_b) ];"        "\n"
        "var sym_p = [ 0x1, 0x200];"                                    "\n"
        "var sym_q : [2]&uint8 = [ &sym_a, cast(&sym_b : &uint8) ];"    "\n"
        "var sym_r : [2]uint16 = [ 0x1, 0x2 ];"                         "\n"
        "var sym_s : [2]uint8 = [ 0x1FF, 0x2FF ];"                      "\n"    // This will fail in the type check pass
        "var sym_t = [ sym_r, sym_s ];"                                 "\n"
        "var sym_u = [ cast(sym_r : [2]uint8), sym_s ];"                "\n"
        "var sym_v = [ &sym_r, &sym_s ];"                               "\n"
        "var sym_w = [ cast(&sym_r : &[2]uint8), &sym_s ];"             "\n"
        "var sym_x = [ sym_w ];"                                        "\n"
        "var sym_y : [][][]&[]uint8 = [ sym_x ];"                       "\n"
        "var sym_z : []uint8 = cast(sym_m : []uint8);"                  "\n"
        "var bool1 : [2]bool = [ false, true ];"                        "\n"
        "var bool2 : []bool = [ true, false ];"                         "\n"
        "var boolarr : [][]bool = [ bool1, bool2 ];"                    "\n"
        "var boolarr2 : []&[]bool = [ &bool1, &bool2 ];"                "\n"
    ;

    const char *tests[][2] = { 
        {   "sym_g",    "[2]uint8",           },
        {   "sym_h",    "[3]uint8",           },
        {   "sym_i",    "[0]customType",      },
        {   "sym_j",    "[1]uint16",          },
        {   "sym_k",    "[1]uint16",          },
        {   "sym_l",    "[2]uint16",          },
        {   "sym_m",    "[3]uint16",          },
        {   "sym_n",    "[1]uint16",          },
        {   "sym_o",    "[2]uint8",           },
        {   "sym_p",    "[2]uint16",          },
        {   "sym_q",    "[2]&uint8",          },
        {   "sym_t",    "[2][2]uint16",       },
        {   "sym_u",    "[2][2]uint8",        },
        {   "sym_v",    "[2]&[2]uint16",      },
        {   "sym_w",    "[2]&[2]uint8",       },
        {   "sym_x",    "[1][2]&[2]uint8",    },
        {   "sym_y",    "[1][1][2]&[2]uint8", },
        {   "sym_z",    "[3]uint8",           },
        {   "bool1",    "[2]bool",            },
        {   "bool2",    "[2]bool",            },
        {   "boolarr",  "[2][2]bool",         },
        {   "boolarr2", "[2]&[2]bool",        },
    };

    const size_t count = sizeof(tests) / sizeof(tests[0]);

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_parsing_valid = zenit_parse_source(&ctx);
    bool is_resolve_valid = is_parsing_valid && zenit_resolve_symbols(&ctx);
    bool is_inference_valid = is_resolve_valid && zenit_infer_types(&ctx);
    
    fl_expect("Parsing, symbol resolving, and type inference phases should not contain errors", is_inference_valid);

    for (size_t i=0; i < count; i++)
    {
        const char **test = tests[i];

        fl_vexpect(zenit_program_has_symbol(ctx.program, test[0]), "Symbol table must contain symbol \"%s\"", test[0]);

        struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx.program, test[0]);

        bool equals = flm_cstring_equals(zenit_type_to_string(symbol->type), test[1]);

        if (equals)
            fl_vexpect(equals, "Symbol %s's type must be %s", test[0], test[1]);
        else
            fl_vexpect(equals, "Symbol %s's type must be %s but actual type is %s", test[0], test[1], zenit_type_to_string(symbol->type));
    }

    zenit_context_free(&ctx);
}

void zenit_test_infer_variables_structs(void)
{
    const char *source = 
        "struct Point { x: uint8; y: uint8; }"                          "\n"
        "var p = Point { x: 0, y: 0 };"                                 "\n"
        "var p2 : Point = { x: 1, y: 1 };"                              "\n"
        "var parr = [ { a: 1 }, { a: 2 }, { a: 3 }, { a: 0x1FF } ];"    "\n"
    ;

    const char *program_dump = 
        "(program"
            " (scope global"
                " (symbol %L2:C20_uint uint8)"
                " (symbol %L2:C26_uint uint8)"
                " (symbol %L2:C9_struct Point)"
                " (symbol p Point)"
                " (symbol %L3:C23_uint uint8)"
                " (symbol %L3:C29_uint uint8)"
                " (symbol %L3:C18_struct { x: uint8, y: uint8 })"   // <- At the inference pass, an unnamed struct remains unnamed...
                " (symbol p2 Point)"
                " (symbol %L4:C19_uint uint8)"
                " (symbol %L4:C14_struct { a: uint8 })"
                " (symbol %L4:C29_uint uint8)"
                " (symbol %L4:C24_struct { a: uint8 })"
                " (symbol %L4:C39_uint uint8)"
                " (symbol %L4:C34_struct { a: uint8 })"
                " (symbol %L4:C49_uint uint16)"
                " (symbol %L4:C44_struct { a: uint16 })"
                " (symbol %L4:C12_array [4]{ a: uint16 })"
                " (symbol parr [4]{ a: uint16 })"
                " (symbol %L3:C18_cast_impl Point)"                 // <- This symbol is the implicit cast from { x: uint8, y: uint8 } to Point
                " (symbol %L4:C14_cast_impl { a: uint16 })"         // <- This symbol is the implicit cast from { a: uint8 } to { a: uint16 }
                " (symbol %L4:C24_cast_impl { a: uint16 })"         // <- This symbol is the implicit cast from { a: uint8 } to { a: uint16 }
                " (symbol %L4:C34_cast_impl { a: uint16 })"         // <- This symbol is the implicit cast from { a: uint8 } to { a: uint16 }
            " (scope struct Point"
                " (symbol x uint8)"
                " (symbol y uint8))))"
    ;

    zenit_test_infer_run(source, program_dump, true);
}
