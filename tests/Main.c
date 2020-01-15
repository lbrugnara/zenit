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
            { "Operators",     &zenit_test_lexer_operators      },
            { "Identifiers",    &zenit_test_lexer_identifiers   },
            { "Keywords",       &zenit_test_lexer_keywords      },
            { "Punctuation",    &zenit_test_lexer_punctuation   },
            { "Errors",         &zenit_test_lexer_errors        },
            { "Combinations",   &zenit_test_lexer_combinations  },
        ),
        fl_test_suite("Parser", 
            { "Simple variable declaration",        &zenit_test_parser_variable_literal                 },
            { "Array variable declaration",         &zenit_test_parser_array_variable_literal           },
            { "Complex variable declaration",       &zenit_test_parser_variable_                        },
            { "Variable decl. with type",           &zenit_test_parser_variable_literal_typeinfo        },
            { "Array Variable decl. with type",     &zenit_test_parser_array_variable_literal_typeinfo  },
            { "Complex variable decl. with type",   &zenit_test_parser_variable_typeinfo                },
            { "Variable decl. errors",              &zenit_test_parser_variable_errors                  },
            { "Integer literals",                   &zenit_test_parser_literal_integer                  },
            { "Integer literal errors",             &zenit_test_parser_literal_integer_error            },
            { "Array initializers",                 &zenit_test_parser_literal_array_literal            },
            { "Variable attributes",                &zenit_test_parser_attributes_variables             },
            { "Struct definition",                  &zenit_test_parser_struct_decl                      },
            { "Struct variables",                   &zenit_test_parser_variable_struct                  },
        ),
        fl_test_suite("Symtable",
            { "Symbol creation",    &zenit_test_symtable_api },
        ),
        fl_test_suite("Resolve",
            { "Resolve too many symbols",                   &zenit_test_resolve_too_many_symbols        },
            { "Resolve variables with primitive types",     &zenit_test_resolve_variables_primitives    },
            { "Resolve variables with reference types",     &zenit_test_resolve_variables_references    },
            { "Resolve variables with array types",         &zenit_test_resolve_variables_arrays        },
            { "Resolve struct declarations",                &zenit_test_resolve_struct_decl             },
            { "Resolve variables with struct types",        &zenit_test_resolve_variables_structs       },
            { "Resolve errors",                             &zenit_test_resolve_errors                  },
        ),
        fl_test_suite("Infer",
            { "Infer variable type using primitives",       &zenit_test_infer_variable_uint         },
            { "Infer variable type using references",       &zenit_test_infer_variable_reference    },
            { "Infer variable type using arrays",           &zenit_test_infer_variable_array        },
            { "Infer variable type using structs",          &zenit_test_infer_variables_structs     },
            { "Inference errors",                           &zenit_test_infer_errors                },
        ),
        fl_test_suite("Check",
            { "Type check success", &zenit_test_check_types_variables   },
            { "Type check errors",  &zenit_test_check_types_errors      }
        ),
        fl_test_suite("zirgen",
            { "Generate ZIR variables",     &zenit_test_generate_ir_variables   },
            { "Generate ZIR casts",         &zenit_test_generate_ir_casts       },
            { "Generate ZIR struct decl",   &zenit_test_generate_ir_struct_decl },
        ),
        fl_test_suite("nes",
            { "NES global variables",           &zenit_test_nes_global_vars         },
            { "NES global variables (ZP)",      &zenit_test_nes_global_vars_zp      },
            { "NES global variables (DATA)",    &zenit_test_nes_global_vars_data    },
            { "NES global variables (CODE)",    &zenit_test_nes_global_vars_code    },
            { "Cast operations",                &zenit_test_nes_cast                },
            { "Compile NES program",            &zenit_test_nes_program             },
            { "Compile NES ROM",                &zenit_test_nes_rom                 },
        ),
        NULL
    );
}
