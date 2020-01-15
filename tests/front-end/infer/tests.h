#ifndef ZENIT_TESTS_INFER_H
#define ZENIT_TESTS_INFER_H

void zenit_test_infer_run(const char *source, const char *test_case, bool verbose);

void zenit_test_infer_variable_uint(void);
void zenit_test_infer_variable_reference(void);
void zenit_test_infer_variable_array(void);
void zenit_test_infer_variables_structs(void);
void zenit_test_infer_errors(void);

#endif /* ZENIT_TESTS_INFER_H */
