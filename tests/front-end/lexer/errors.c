

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) ZENIT_TOKEN_##token

static struct ZenitLexerTest tests[] = {
    // Errors
    { "123$invalid_id",             (ZenitTokenType[]){ T(INTEGER), T(UNKNOWN), T(EOF) } },
    { "var err = 123$invalid_id;",  (ZenitTokenType[]){ T(VAR), T(ID), T(ASSIGN), T(INTEGER), T(UNKNOWN), T(SEMICOLON), T(EOF) } },
};

void zenit_test_lexer_errors(void)
{
    zenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
