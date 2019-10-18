#include <fllib.h>
#include <stdio.h>
#include <locale.h>
#include <time.h>

#include "Test.h"

// Tests
#include "front-end/lexer/tests.h"
#include "front-end/parser/tests.h"
#include "front-end/resolve/tests.h"
#include "front-end/symtable/tests.h"

int main(int argc, char **argv) 
{
    fl_test_run_all_suites(
        argc,
        argv,
        fl_test_suite("Lexer", 
            { "Types",          &cenit_test_lexer_types         },
            { "Assignment",     &cenit_test_lexer_assignment    },
            { "Identifiers",    &cenit_test_lexer_identifiers   },
            { "Keywords",       &cenit_test_lexer_keywords      },
            { "Punctuation",    &cenit_test_lexer_punctuation   },
            { "Errors",         &cenit_test_lexer_errors        },
            { "Combinations",   &cenit_test_lexer_combinations  },
        ),
        fl_test_suite("Parser", 
            { "Variable declaration",           &cenit_test_parser_variable                     },
            { "Array variable declaration",     &cenit_test_parser_array_variable               },
            { "Variable decl. with type",       &cenit_test_parser_variable_type_info           },
            { "Array Variable decl. with type", &cenit_test_parser_array_variable_type_info     },
            { "Variable decl. errors",          &cenit_test_parser_variable_errors              },
            { "Integer literals",               &cenit_test_parser_literal_integer              },
            { "Integer literal errors",         &cenit_test_parser_literal_integer_error        },
            { "Array initializers",             &cenit_test_parser_literal_array_initializer    },
        ),
        fl_test_suite("Symtable",
            { "Symbol creation",    &cenit_test_symtable_api },
        ),
        fl_test_suite("Resolve",
            { "Resolve variables",  &cenit_test_resolve_variables },
        ),
        NULL
    );
}
