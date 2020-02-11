

#include "../../Test.h"
#include "../../../src/front-end/ast/ast.h"
#include "../../../src/front-end/phases/parse.h"
#include "tests.h"

void zenit_test_parser_variable_literal(void)
{
    const char *source = 
        "var num0 = 0;"
        "var num1 = 1;"
        "var num2 = 2;"
        "var num3 = 3;"
        "var bool1 = true;"
        "var bool2 = false;"
    ;

    const char *ast_dump = 
        "(ast"
        " (var num0 (uint8 0))"
        " (var num1 (uint8 1))"
        " (var num2 (uint8 2))"
        " (var num3 (uint8 3))"
        " (var bool1 (bool true))"
        " (var bool2 (bool false))"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}

void zenit_test_parser_array_variable_literal(void)
{
    const char *source = 
        "var num0 = [];"
        "var num1 = [ 0 ];"
        "var num2 = [ 1 ];"
        "var num3 = [ 2 ];"
        "var num4 = [ 3, 4 ];"
        "var bool1 = [ true, false ];"
    ;

    const char *ast_dump = 
        "(ast"
        " (var num0 (array))"
        " (var num1 (array (uint8 0)))"
        " (var num2 (array (uint8 1)))"
        " (var num3 (array (uint8 2)))"
        " (var num4 (array (uint8 3) (uint8 4)))"
        " (var bool1 (array (bool true) (bool false)))"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}

void zenit_test_parser_variable_literal_type(void)
{
    const char *source = 
        "var num0 : uint8 = 0;"
        "var num1 : uint8 = 1;"
        "var num2 : uint8 = 2;"
        "var num3 : uint8 = 3;"
        "var num4 : uint16 = 4;"
        "var bool1 : bool = true;"
        "var bool2 : bool = false;"
    ;

    const char *ast_dump = 
        "(ast"
        " (var num0 (type uint8) (uint8 0))"
        " (var num1 (type uint8) (uint8 1))"
        " (var num2 (type uint8) (uint8 2))"
        " (var num3 (type uint8) (uint8 3))"
        " (var num4 (type uint16) (uint8 4))"
        " (var bool1 (type bool) (bool true))"
        " (var bool2 (type bool) (bool false))"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}


void zenit_test_parser_array_variable_literal_type(void)
{
    const char *source = 
        "var num0 : [0]uint8 = [];"
        "var num1 : [1]uint8 = [ 0 ];"
        "var num2 : [2]uint8 = [ 1, 2 ];"
        "var num3 : [3]uint8 = [ 3, 4, 5 ];"
        "var num4 : [4]uint8 = [ 6, 7, 8, 9 ];"
        "var num5 : []uint8 = [ 10, 11, 12, 13, 14 ];"
        "var bool1 : [2]bool = [ false, true ];"
        "var bool2 : []bool = [ true, false ];"
        "var boolarr : [][]bool = [ bool1, bool2 ];"
        "var boolarr2 : [][]&bool = [ &bool1, &bool2 ];"
    ;

    const char *ast_dump = 
        "(ast"
        " (var num0 (type [0]uint8) (array))"
        " (var num1 (type [1]uint8) (array (uint8 0)))"
        " (var num2 (type [2]uint8) (array (uint8 1) (uint8 2)))"
        " (var num3 (type [3]uint8) (array (uint8 3) (uint8 4) (uint8 5)))"
        " (var num4 (type [4]uint8) (array (uint8 6) (uint8 7) (uint8 8) (uint8 9)))"
        " (var num5 (type []uint8) (array (uint8 10) (uint8 11) (uint8 12) (uint8 13) (uint8 14)))"
        " (var bool1 (type [2]bool) (array (bool false) (bool true)))"
        " (var bool2 (type []bool) (array (bool true) (bool false)))"
        " (var boolarr (type [][]bool) (array (id bool1) (id bool2)))"
        " (var boolarr2 (type [][]&bool) (array (ref (id bool1)) (ref (id bool2))))"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}

void zenit_test_parser_variable_ref(void)
{
    const char *source = 
        "var num0 = some_extern_id;"
        "var num1 = num0;"
        "var num2 = &num1;"
        "var bool1 = true;"
        "var bref = &bool1;"
    ;

    const char *ast_dump =
        "(ast"
        " (var num0 (id some_extern_id))"
        " (var num1 (id num0))"
        " (var num2 (ref (id num1)))"
        " (var bool1 (bool true))"
        " (var bref (ref (id bool1)))"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}

void zenit_test_parser_variable_ref_type(void)
{
    const char *source = 
        "var num0 : uint8 = some_extern_id;"
        "var num1 : uint8 = num0;"
        "var num2 : &uint8 = &num1;"
        "var bool1 : bool = true;"
        "var bref : &bool = &bool1;"
    ;

    const char *ast_dump =
        "(ast"
        " (var num0 (type uint8) (id some_extern_id))"
        " (var num1 (type uint8) (id num0))"
        " (var num2 (type &uint8) (ref (id num1)))"
        " (var bool1 (type bool) (bool true))"
        " (var bref (type &bool) (ref (id bool1)))"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}

void zenit_test_parser_variable_struct(void)
{
    const char *source = 
        "struct Point { x: uint8; y: uint8; }"              "\n"
        "var p = Point { x: 0, y: 0 };"                     "\n"
        "var p2 : Point = { x: 1, y: 1 };"                  "\n"
    ;

    const char *ast_dump =
        "(ast"
        " (struct-decl Point (field x (type uint8)) (field y (type uint8)))"
        " (var p (struct Point (x (uint8 0)) (y (uint8 0))))"
        " (var p2 (type Point) (struct (x (uint8 1)) (y (uint8 1))))"
        ")"
    ;

    zenit_test_parser_run(source, ast_dump);
}
