#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/phases/check.h"
#include "../../../src/front-end/phases/infer.h"
#include "../../../src/front-end/phases/parse.h"
#include "../../../src/front-end/phases/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void zenit_test_check_types_variable_errors(void)
{
    const char *source = 
        "var sym_a : [0]custom = [];"                           "\n"
        
        "var sym_b : uint8 = [ 1, 2 ];"                         "\n"
        
        "var sym_c : [2]uint8 = 0;"                             "\n"
        
        "var sym_d : uint8 = cast(1 : uint16);"                 "\n"
        
        "var sym_e = &&sym_b;"                                  "\n"
        
        "var sym_f : [2]uint8 = [ 0x1FF, 0x200];"               "\n"
        
        "#[NES(address: 0x10)]"                                 "\n"
        "var player = 1;"                                       "\n"
        "var address = 0x10;"                                   "\n"
        "var player_ref : &uint8 = cast(address : &uint8);"     "\n"

        "var sym_h : [2]uint8 = [ 1, sym_a ];"                  "\n"

        "var a = 0x20;"                                         "\n"
        "var b = [ 0x1, 0x2, &a ];"                             "\n"
        
        "var c : [1]uint8 = [ 0x1, 0x2 ];"                      "\n"
        
        // The cast can be inferred from the type hint
        "var d : [1]uint8 = [ 0x1, cast(0x1FF) ];"              "\n"
        
        "var e : [1]&uint8 = [ 0x1 ];"                          "\n"

        // cast(0x1 : &uint8) is inferred because of the type hint
        "var f : [1]&uint8 = [ cast(0x1) ];"                    "\n"

        // cast(0x1FF : &uint8) is inferred because of the type hint
        "var g : [2]&uint8 = [ cast(0x1FF) ];"                  "\n"

        // cast(0x1FF : &uint8) is inferred because of the type hint
        "var h : [2]uint8 = [ cast(0x1FF) ];"                   "\n"

        "var i : &[1]uint8 = &a;"                               "\n"

        "var j = [ 0x1FF, 0x200, 0x201];"                       "\n"
        "var k : &[3]uint8 = &j;"                               "\n"

        // Cannot cast a reference from a smaller type to a larger type
        "var l = cast(&a : &uint16);"                           "\n"

        "var m : [2]uint8 = [ 0x1, 0x2 ];"                      "\n"
        "var n : [2]uint16 = [ 0x1FF, 0x2FF ];"                 "\n"
        "var o = [ &m, &n ];"                                   "\n"
    ;

    struct ExpectedError expected_errors[] = {
        { ZENIT_ERROR_MISSING_SYMBOL, "'custom' type is not defined (<source>:%u:%u: %s)" },
        
        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot assign [2]uint8 to uint8 (<source>:%u:%u: %s)" },
        
        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot assign uint8 to [2]uint8 (<source>:%u:%u: %s)" },
        
        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot assign uint16 to uint8 (<source>:%u:%u: %s)" },
        
        { ZENIT_ERROR_INVALID_REFERENCE, "Cannot take a reference to a reference (<source>:%u:%u: %s)" },
        
        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot convert from [2]uint16 to [2]uint8 (<source>:%u:%u: %s)" },
        
        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot cast from uint8 to &uint8 (<source>:%u:%u: %s)" },

        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot cast from [0]custom to uint8 (<source>:%u:%u: %s)" },

        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot convert from &uint8 to uint8 (<source>:%u:%u: %s)" },
        
        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot convert from [2]uint8 to [1]uint8 (<source>:%u:%u: %s)" },
        
        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot convert from [2]uint8 to [1]uint8 (<source>:%u:%u: %s)" },
        
        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot convert from uint8 to &uint8 (<source>:%u:%u: %s)" },
        
        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot convert from uint8 to &uint8 (<source>:%u:%u: %s)" },

        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot cast from uint16 to &uint8 (<source>:%u:%u: %s)" },
        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot assign from [1]&uint8 to [2]&uint8 (<source>:%u:%u: %s)" },

        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot assign from [1]&uint8 to [2]&uint8 (<source>:%u:%u: %s)" },

        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot assign from &uint8 to &[1]uint8 (<source>:%u:%u: %s)" },

        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot assign from &[3]uint16 to &[3]uint8 (<source>:%u:%u: %s)" },

        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot cast from &uint8 to &uint16 (<source>:%u:%u: %s)" },
    };

    zenit_test_check_type_errors(source, expected_errors, sizeof(expected_errors) / sizeof(expected_errors[0]));
}

void zenit_test_check_types_struct_errors(void)
{
    const char *source =
        "struct A { x: uint8; y: uint8; }"                  "\n"
        "struct B { x: uint8; y: uint8; z: uint8; }"        "\n"
        "var a = A { x: 0x1FF, y: [ 0x2FF ] };"             "\n"
        "var b : B = A { x: 0, y: 0 };"                     "\n"
        "struct C { a: A; }"                                "\n"
        "var c = C { a: B { x: 0, y: 1, z: 2 } }; "         "\n"
    ;
    struct ExpectedError expected_errors[] = {
        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot convert from uint16 to uint8"     },
        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot convert from [1]uint16 to uint8"  },
        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot convert from A to B"              },
        { ZENIT_ERROR_TYPE_MISSMATCH, "Cannot convert from B to A"              },
    };

    zenit_test_check_type_errors(source, expected_errors, sizeof(expected_errors) / sizeof(expected_errors[0]));
}
