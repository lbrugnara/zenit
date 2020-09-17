

#include <flut/flut.h>
#include "../../../src/front-end/inference/infer.h"
#include "../../../src/front-end/parser/parse.h"
#include "../../../src/front-end/program.h"
#include "../../../src/front-end/types/context.h"
#include "../../../src/front-end/binding/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void zenit_test_infer_if_statement(void)
{
    const char *source = 
        "var x = 1;"                                            "\n"
        "if (cast(x)) {}"                                       "\n"
        "if (cast(x)) {} else {}"                               "\n"
        "if (cast(x)) { var a = x; } else { var b = x; }"       "\n"
    ;

    const char *program_dump = 
        "(program "
            "(scope global "
                "(symbol %L1:C9_uint uint8) "       // The "1" literal
                "(symbol x uint8) "                 // The "var x" definition
                "(scope block %L2:C1_if "           // The "if" statement
                    "(symbol %L2:C5_cast bool)"     // The "cast(x)" expression (the "if" condition)
                    " "
                    "(scope block %L2:C14_block)"   // The "then" branch of the "if" statement
                ")"
                " "
                "(scope block %L3:C1_if "           // The "if" statement
                    "(symbol %L3:C5_cast bool)"     // The "cast(x)" expression (the "if" condition)
                    " "
                    "(scope block %L3:C14_block)"   // The "then" branch of the "if" statement
                    " "
                    "(scope block %L3:C22_block)"   // The "else" branch of the "if" statement
                ")"
                " "
                "(scope block %L4:C1_if "           // The "if" statement
                    "(symbol %L4:C5_cast bool)"     // The "cast(x)" expression (the "if" condition)
                    " "
                    "(scope block %L4:C14_block "   // The "then" branch of the "if" statement
                        "(symbol a uint8)"
                    ")"
                    " "
                    "(scope block %L4:C34_block "   // The "else" branch of the "if" statement
                        "(symbol b uint8)"
                    ")"
                ")"
            ")"
        ")"
    ;

    zenit_test_infer_run(source, program_dump, true);
}
