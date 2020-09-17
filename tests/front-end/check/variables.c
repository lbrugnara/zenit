#include <stdio.h>


#include <flut/flut.h>
#include "../../../src/front-end/type-check/check.h"
#include "../../../src/front-end/inference/infer.h"
#include "../../../src/front-end/parser/parse.h"
#include "../../../src/front-end/binding/resolve.h"
#include "../../../src/front-end/program.h"
#include "tests.h"

void zenit_test_check_types_array(void)
{
    const char *source = 
        "var a = [ [ 3, 4 ], [ 5, 6 ], [ 7, 8 ] ];"                     "\n"
        "var a_ref = &a;"                                               "\n"
        "var a_ref_ref = &a_ref;"                                       "\n"
        "var sym_t : [1]uint8 = [ cast(0x1FF) ];"                       "\n"
        
        "var l = [ 1, 2, 3];"                                           "\n"
        "var m : &[3]uint16 = &l;"                                      "\n"
        "var n : &[3]uint8 = &l;"                                       "\n"

        "var o = [ 0x1FF, 0x2FF, 0x3FF];"                               "\n"
        "var p : &[3]uint8 = cast(&o);"                                 "\n"
        "var q : &[3]uint16 = &o;"                                      "\n"

        "var r = [ 0x1, 0x2 ];"                                         "\n"
        "var s : [2]uint16 = r;"                                        "\n"

        "var t : []&[][]uint8 = [ &a, a_ref ];"                         "\n"

        "var u : [0]Astruct = [];"                                      "\n"
        "var v : [0]Astruct = u;"                                       "\n"
        "var w = u;"                                                    "\n"

        "struct Astruct { a: uint8; }"                                  "\n" // No need to forward declare the struct

        "var b1 : []bool = [];"                                         "\n"
        "var b2 : []bool = [ true, false ];"                            "\n"
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

        { "b1",         "[0]bool"           },
        { "b2",         "[2]bool"           },
    };

    zenit_test_check_types(source, tests, sizeof(tests) / sizeof(tests[0]));
}

void zenit_test_check_types_struct(void)
{
    const char *source = 
        "struct Astruct { a: uint8; }"                                  "\n" // No need to forward declare the struct

        "var a1 = Astruct { a: 2 };"                                    "\n"
        "var a2 : Astruct = { a: 2 };"                                  "\n"
        "var a3 = a1;"                                                  "\n"
        "var a4 = a2;"                                                  "\n"

        "struct Bstruct { a: Astruct; }"                                "\n"
        "var b1 = Bstruct { a: { a: 2 } };"                             "\n"
        "var b2 : Bstruct = { a: { a: 2 } };"                           "\n"
        "var b3 = Bstruct { a: Astruct { a: 2 } };"                     "\n"
        "var b4 : Bstruct = { a: Astruct { a: 2 } };"                   "\n"

        "struct Cstruct { a: Astruct; b: Bstruct; c: uint8; }"          "\n"
        "var c1 = Cstruct { a: { a: 2 }, b: { a: { a: 3} }, c: 1 };"    "\n"

        "var d1 = { a: 1, b: 0x1FF, c: { x: 1 } };"                     "\n"
        "var d2 = d1;"                                                  "\n"
        "var d3 = &d1;"                                                 "\n"

        "var parr = [ { a: 1 }, { a: 2 }, { a: 3 }, { a: 0x1FF } ];"    "\n"
    ;

    const char *tests[][2] = {
        { "a1",          "Astruct"                                      },
        { "a2",          "Astruct"                                      },
        { "a3",          "Astruct"                                      },
        { "a4",          "Astruct"                                      },

        { "b1",          "Bstruct"                                      },
        { "b2",          "Bstruct"                                      },
        { "b3",          "Bstruct"                                      },
        { "b4",          "Bstruct"                                      },

        { "c1",          "Cstruct"                                      },

        { "d1",          "{ a: uint8, b: uint16, c: { x: uint8 } }"     },
        { "d2",          "{ a: uint8, b: uint16, c: { x: uint8 } }"     },
        { "d3",          "&{ a: uint8, b: uint16, c: { x: uint8 } }"    },

        { "parr",        "[4]{ a: uint16 }"                             },
    };

    zenit_test_check_types(source, tests, sizeof(tests) / sizeof(tests[0]));
}
