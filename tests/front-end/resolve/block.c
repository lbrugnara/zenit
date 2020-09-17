

#include <flut/flut.h>
#include "../../../src/front-end/parser/parse.h"
#include "../../../src/front-end/program.h"
#include "../../../src/front-end/binding/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/types/context.h"
#include "tests.h"

void zenit_test_resolve_block(void)
{
    const char *source = 
        "{"                                 "\n"
        "   var a = 1;"                     "\n"
        "   {"                              "\n"
        "       var b = 2;"                 "\n"
        "       {"                          "\n"
        "           var c = 3;"             "\n"
        "       }"                          "\n"
        "   }"                              "\n"
        "}"                                 "\n"
    ;

    const char *program_dump = 
        "(program "
            "(scope global "
                "(scope block %L1:C1_block "
                    "(symbol a uint8)"
                    " "
                    "(scope block %L3:C4_block "
                        "(symbol b uint8)"
                        " "
                        "(scope block %L5:C8_block "
                            "(symbol c uint8)"
                        ")"
                    ")"
                ")"
            ")"
        ")"
    ;

    zenit_test_resolve_run(source, program_dump, false);
}
