#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/passes/parse.h"
#include "../../../src/front-end/passes/utils.h"
#include "../../../src/front-end/program.h"
#include "../../../src/front-end/passes/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/types/system.h"
#include "tests.h"

#include "symbols.h"

void zenit_test_resolve_too_many_symbols(void)
{
    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_FILE, "tests/front-end/resolve/symbols.zt");
    bool is_valid = zenit_parse_source(&ctx);

    zenit_resolve_symbols(&ctx);

    size_t number_of_symbols = 500;
    for (size_t i=0; i < number_of_symbols; i++)
        fl_vexpect(zenit_program_has_symbol(ctx.program, symbols[i]), "Symbol '%s' must exist", symbols[i]);

    zenit_context_free(&ctx);
}

void zenit_test_resolve_variables_primitives(void)
{
    const char *source = 
        "var a : uint8 = 2;"                "\n"
        "var b = 1;"                        "\n"
        "var c : uint16 = 0x1FF;"           "\n"
        "var d = b;"                    "\n"
    ;

    const char *program_dump = 
        "(program "
            "(scope global"
                " (symbol a uint8)"
                " (symbol b uint8)"
                " (symbol c uint16)"
                " (symbol d uint8)"
            ")"
        ")"
    ;

    zenit_test_resolve_run(source, program_dump, false);
}

void zenit_test_resolve_variables_references(void)
{
    const char *source = 
        "var a = 2;"                    "\n"
        "var b = &a;"                   "\n"
        "var c : &uint8 = &a;"          "\n"
        "var d : uint16 = 0x1FF;"       "\n"
        "var e : &uint16 = &d;"         "\n"
        "var f : [2]uint8 = [ 0, 1 ];"  "\n"
        "var g : &[2]uint8 = &f;"       "\n"
        "var h : [1]&[2]uint8 = [ g ];" "\n"
        "var i : []&[2]uint8 = [ g ];"  "\n"
        "var j : []&[]uint8 = [ g ];"   "\n"

        "var k : []uint16 = [ 0x1, 0x2 ];"      "\n"
        "var l : []uint8 = [ 0x1FF, 0x2FF ];"   "\n"
        "var m = [ cast(&k : &[2]uint8), &l ];" "\n"
        "var n = [ m ];"                        "\n"
        "var o : [][][]&[]uint8 = [ n ];"       "\n"
    ;

    const char *program_dump = 
        "(program "
            "(scope global"
                " (symbol a uint8)"
                " (symbol b &uint8)"
                " (symbol c &uint8)"
                " (symbol d uint16)"
                " (symbol e &uint16)"
                " (symbol f [2]uint8)"
                " (symbol g &[2]uint8)"
                " (symbol h [1]&[2]uint8)"
                " (symbol i [1]&[2]uint8)"
                " (symbol j [1]&[2]uint8)"
                " (symbol k [2]uint16)"
                " (symbol l [2]uint8)"
                " (symbol m [2]&[2]uint8)"
                " (symbol n [1][2]&[2]uint8)"
                " (symbol o [1][1][2]&[2]uint8)"
            ")"
        ")"
    ;

    zenit_test_resolve_run(source, program_dump, false);
}

void zenit_test_resolve_variables_arrays(void)
{
    const char *source = 
        "var a : [2]uint8 = [ 1, 2 ];"      "\n"
        "var b = [ 1, 2, 3 ];"              "\n"
        "var c : [0]customType = [];"       "\n"
        "var d : uint16 = 1;"               "\n"
        "var e : [1]uint16 = [ d ];"        "\n"
    ;

    const char *program_dump = 
        "(program "
            "(scope global"
                " (symbol a [2]uint8)"
                " (symbol b [3]uint8)"
                " (symbol c [0]customType)"
                " (symbol d uint16)"
                " (symbol e [1]uint16)"
            ")"
        ")"
    ;

    zenit_test_resolve_run(source, program_dump, false);
}

void zenit_test_resolve_variables_structs(void)
{
    const char *source = 
        "struct Point { x: uint8; y: uint8; }"              "\n"
        "var p = Point { x: 0, y: 0 };"                     "\n"
        "var p2 : Point = { x: 1, y: 1 };"                  "\n"
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
                " (symbol %L3:C18_struct 'a)" // <- The struct to be assigned to p2 is an unnamed struct after running the resolve pass
                " (symbol p2 Point)"
            " (scope struct Point"
                " (symbol x uint8)"
                " (symbol y uint8))))"
    ;

    zenit_test_resolve_run(source, program_dump, true);
}
