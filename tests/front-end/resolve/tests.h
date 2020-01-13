#ifndef ZENIT_TESTS_RESOLVE_H
#define ZENIT_TESTS_RESOLVE_H

void zenit_test_resolve_run(const char *source, const char *ast_dump);

void zenit_test_resolve_too_many_symbols(void);
void zenit_test_resolve_primitives(void);
void zenit_test_resolve_references(void);
void zenit_test_resolve_arrays(void);
void zenit_test_resolve_errors(void);
void zenit_test_resolve_struct_declaration(void);

#endif /* ZENIT_TESTS_RESOLVE_H */
