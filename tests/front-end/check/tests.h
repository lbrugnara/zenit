#ifndef ZENIT_TESTS_CHECK_H
#define ZENIT_TESTS_CHECK_H

#include "../../../src/front-end/context.h"

struct ExpectedError {
    enum ZenitErrorType type;
    const char *message;
};

void zenit_test_check_types(const char *source, const char *cases[][2], size_t length);
void zenit_test_check_type_errors(const char *source, struct ExpectedError *cases, size_t length);

void zenit_test_check_types_struct(void);
void zenit_test_check_types_array(void);
void zenit_test_check_types_variable_errors(void);
void zenit_test_check_types_struct_errors(void);

#endif /* ZENIT_TESTS_CHECK_H */
