

#include <flut/flut.h>
#include "../../../src/front-end/parser/parse.h"
#include "../../../src/front-end/phases/utils.h"
#include "../../../src/front-end/program.h"
#include "../../../src/front-end/phases/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/types/context.h"
#include "tests.h"

void zenit_test_resolve_if_statement(void)
{
    const char *source = 
        "if (true) {}"                                      "\n"
        "if (true) {} else {}"                              "\n"
        "if (true) { var a = 1; } else { var b = 2; }"      "\n"
    ;

    const char *program_dump = 
        "(program "
            "(scope global "
                "(scope block %L1:C1_if "
                    "(scope block %L1:C11_block)"
                ")"
                " "
                "(scope block %L2:C1_if "
                    "(scope block %L2:C11_block)"
                    " "
                    "(scope block %L2:C19_block)"
                ")"
                " "
                "(scope block %L3:C1_if "
                    "(scope block %L3:C11_block "
                        "(symbol a uint8)"
                    ")"
                    " "
                    "(scope block %L3:C31_block "
                        "(symbol b uint8)"
                    ")"
                ")"
            ")"
        ")"
    ;

    zenit_test_resolve_run(source, program_dump, false);
}
