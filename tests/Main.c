
#include <stdio.h>
#include <locale.h>
#include <time.h>

#include <flut/flut.h>

// Tests
#include "front-end/check/tests.h"
#include "front-end/infer/tests.h"
#include "front-end/lexer/tests.h"
#include "front-end/parser/tests.h"
#include "front-end/resolve/tests.h"
#include "front-end/symtable/tests.h"
#include "zir/tests.h"
#include "back-end/nes/tests.h"

int main(int argc, char **argv) 
{
    flut_run_tests(
        argc,
        argv,
        flut_suite("Lexer", 
            { "Types",          &zenit_test_lexer_types         },
            { "Operators",      &zenit_test_lexer_operators     },
            { "Identifiers",    &zenit_test_lexer_identifiers   },
            { "Keywords",       &zenit_test_lexer_keywords      },
            { "Punctuation",    &zenit_test_lexer_punctuation   },
            { "Errors",         &zenit_test_lexer_errors        },
            { "Combinations",   &zenit_test_lexer_combinations  },
            { "Comments",       &zenit_test_lexer_comments      },
        ),
        flut_suite("Parser", 
            { "Simple variable declaration",            &zenit_test_parser_variable_literal             },
            { "Array variable declaration",             &zenit_test_parser_array_variable_literal       },
            { "Variable decl. with type",               &zenit_test_parser_variable_literal_type        },
            { "Array Variable decl. with type",         &zenit_test_parser_array_variable_literal_type  },
            { "Variable decl. errors",                  &zenit_test_parser_variable_errors              },
            { "Reference variables declarations",       &zenit_test_parser_variable_ref                 },
            { "Reference variables decl. with type",    &zenit_test_parser_variable_ref                 },
            { "Integer literals",                       &zenit_test_parser_literal_integer              },
            { "Integer literal errors",                 &zenit_test_parser_literal_integer_error        },
            { "Boolean literals",                       &zenit_test_parser_literal_boolean              },
            { "Array initializers",                     &zenit_test_parser_literal_array_literal        },
            { "Variable attributes",                    &zenit_test_parser_attributes_variables         },
            { "Struct definition",                      &zenit_test_parser_struct_decl                  },
            { "Struct variables",                       &zenit_test_parser_variable_struct              },
            { "Parse blocks",                           &zenit_test_parser_blocks                       },
            { "Parse if statements",                    &zenit_test_parser_if_statements                },
        ),
        flut_suite("Symtable",
            { "Symbol creation",    &zenit_test_symtable_api },
        ),
        flut_suite("Resolve",
            { "Resolve too many symbols",                   &zenit_test_resolve_too_many_symbols        },
            { "Resolve variables with primitive types",     &zenit_test_resolve_variables_primitives    },
            { "Resolve variables with reference types",     &zenit_test_resolve_variables_references    },
            { "Resolve variables with array types",         &zenit_test_resolve_variables_arrays        },
            { "Resolve struct declarations",                &zenit_test_resolve_struct_decl             },
            { "Resolve variables with struct types",        &zenit_test_resolve_variables_structs       },
            { "Resolve variable errors",                    &zenit_test_resolve_variable_errors         },
            { "Resolve struct errors",                      &zenit_test_resolve_struct_errors           },
            { "Resolve if statement",                       &zenit_test_resolve_if_statement            },
            { "Resolve block",                              &zenit_test_resolve_block                   },
            { "Resolve block errors",                       &zenit_test_resolve_block_errors            },
        ),
        flut_suite("Infer",
            { "Infer variable type using primitives",       &zenit_test_infer_variable_uint         },
            { "Infer variable type using references",       &zenit_test_infer_variable_reference    },
            { "Infer variable type using arrays",           &zenit_test_infer_variable_array        },
            { "Infer variable type using structs",          &zenit_test_infer_variables_structs     },
            { "Infer in if statement",                      &zenit_test_infer_if_statement          },
            { "Inference errors",                           &zenit_test_infer_errors                },
        ),
        flut_suite("Check",
            { "Type check arrays",          &zenit_test_check_types_array               },
            { "Type check structs",         &zenit_test_check_types_struct              },
            { "Type check variable errors", &zenit_test_check_types_variable_errors     },
            { "Type check struct errors",   &zenit_test_check_types_struct_errors       },
        ),
        flut_suite("zir",
            { "Generate ZIR variables",         &zenit_test_generate_ir_variables       },
            { "Generate ZIR variable clash",    &zenit_test_generate_ir_variables_clash },
            { "Generate ZIR casts",             &zenit_test_generate_ir_casts           },
            { "Generate ZIR struct decl",       &zenit_test_generate_ir_struct_decl     },
            { "Generate ZIR struct",            &zenit_test_generate_ir_struct          },
            { "Generate ZIR if",                &zenit_test_generate_ir_if              },
        ),
        flut_suite("nes",
            { "NES global variables",               &zenit_test_nes_global_vars             },
            { "NES global array variables",         &zenit_test_nes_global_vars_array       },
            { "NES global variables (ZP)",          &zenit_test_nes_global_vars_zp          },
            { "NES global variables (DATA)",        &zenit_test_nes_global_vars_data        },
            { "NES global variables (CODE)",        &zenit_test_nes_global_vars_code        },
            { "NES global variables name clash",    &zenit_test_nes_global_var_name_clash   },
            { "Cast operations",                    &zenit_test_nes_cast                    },
            { "Compile NES program",                &zenit_test_nes_program                 },
            { "Compile NES ROM",                    &zenit_test_nes_rom                     },
        ),
        NULL
    );
}
