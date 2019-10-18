#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) CENIT_TOKEN_##token

static struct CenitLexerTest tests[] = {
    // Errors
    { "123$invalid_id", true,   (CenitTokenType[]){ T(INTEGER), T(UNKNOWN), T(EOF) } },
    { "var err = 123$invalid_id;", true,   (CenitTokenType[]){ T(VAR), T(ID), T(ASSIGNMENT), T(INTEGER), T(UNKNOWN), T(SEMICOLON), T(EOF) } },
};

void cenit_test_lexer_errors(void)
{
    cenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
