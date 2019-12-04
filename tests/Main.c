#include <fllib.h>
#include <stdio.h>
#include <locale.h>
#include <time.h>

#include "Test.h"

// Tests
#include "front-end/check/tests.h"
#include "front-end/infer/tests.h"
#include "front-end/lexer/tests.h"
#include "front-end/parser/tests.h"
#include "front-end/resolve/tests.h"
#include "front-end/symtable/tests.h"
#include "front-end/zirgen/tests.h"
#include "back-end/nes/tests.h"

int main(int argc, char **argv) 
{
    fl_test_run_all_suites(
        argc,
        argv,
        fl_test_suite("Lexer", 
            { "Types",          &zenit_test_lexer_types         },
            { "Operators",     &zenit_test_lexer_operators    },
            { "Identifiers",    &zenit_test_lexer_identifiers   },
            { "Keywords",       &zenit_test_lexer_keywords      },
            { "Punctuation",    &zenit_test_lexer_punctuation   },
            { "Errors",         &zenit_test_lexer_errors        },
            { "Combinations",   &zenit_test_lexer_combinations  },
        ),
        fl_test_suite("Parser", 
            { "Simple variable declaration",        &zenit_test_parser_variable_literal                 },
            { "Array variable declaration",         &zenit_test_parser_array_variable_literal           },
            { "Complex variable declaration",       &zenit_test_parser_variable_complex                 },
            { "Variable decl. with type",           &zenit_test_parser_variable_literal_typeinfo        },
            { "Array Variable decl. with type",     &zenit_test_parser_array_variable_literal_typeinfo  },
            { "Complex variable decl. with type",   &zenit_test_parser_variable_complex_typeinfo        },
            { "Variable decl. errors",              &zenit_test_parser_variable_errors                  },
            { "Integer literals",                   &zenit_test_parser_literal_integer                  },
            { "Integer literal errors",             &zenit_test_parser_literal_integer_error            },
            { "Array initializers",                 &zenit_test_parser_literal_array_initializer        },
            { "Variable attributes",                &zenit_test_parser_attributes_variables             },
        ),
        fl_test_suite("Symtable",
            { "Symbol creation",    &zenit_test_symtable_api },
        ),
        fl_test_suite("Resolve",
            { "Resolve variables",  &zenit_test_resolve_variables   },
            { "Resolve errors",     &zenit_test_resolve_errors      },
        ),
        fl_test_suite("Infer",
            { "Infer variable type",    &zenit_test_infer_variable_type },
            { "Inference errors",       &zenit_test_infer_errors        },
        ),
        fl_test_suite("Check",
            { "Type check errors",  &zenit_test_check_types_errors  }
        ),
        fl_test_suite("zirgen",
            { "Generate ZIR variables",  &zenit_test_generate_zenit_ir_variables  }
        ),
        fl_test_suite("nes",
            { "NES global variables",  &zenit_test_nes_global_variables  }
        ),
        NULL
    );
}
