#ifndef ZENIT_TESTS_PARSER_H
#define ZENIT_TESTS_PARSER_H

void zenit_test_parser_run(const char *source, const char *ast_dump);

void zenit_test_parser_attributes_variables(void);
void zenit_test_parser_variable_literal(void);
void zenit_test_parser_variable_ref(void);
void zenit_test_parser_variable_ref_type(void);
void zenit_test_parser_variable_literal_type(void);
void zenit_test_parser_variable_errors(void);
void zenit_test_parser_variable_struct(void);
void zenit_test_parser_array_variable_literal(void);
void zenit_test_parser_array_variable_literal_type(void);
void zenit_test_parser_literal_integer(void);
void zenit_test_parser_literal_integer_error(void);
void zenit_test_parser_literal_boolean(void);
void zenit_test_parser_literal_array_literal(void);
void zenit_test_parser_struct_decl(void);
void zenit_test_parser_blocks(void);
void zenit_test_parser_if_statements(void);

#endif /* ZENIT_TESTS_PARSER_H */
