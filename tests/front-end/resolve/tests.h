#ifndef ZENIT_TESTS_RESOLVE_H
#define ZENIT_TESTS_RESOLVE_H

#include "../../../src/front-end/context.h"

struct ResolveTestCase {
    ZenitErrorType error;
    const char *message;
};

void zenit_test_resolve_run(const char *source, const char *ast_dump, bool verbose);
void zenit_test_resolve_errors(const char *source, struct ResolveTestCase *tests, size_t tests_count);

void zenit_test_resolve_too_many_symbols(void);
void zenit_test_resolve_variables_primitives(void);
void zenit_test_resolve_variables_references(void);
void zenit_test_resolve_variables_arrays(void);
void zenit_test_resolve_variable_errors(void);
void zenit_test_resolve_struct_errors(void);
void zenit_test_resolve_struct_decl(void);
void zenit_test_resolve_variables_structs(void);

#endif /* ZENIT_TESTS_RESOLVE_H */
