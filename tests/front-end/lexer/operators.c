#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) ZENIT_TOKEN_##token

static struct ZenitLexerTest tests[] = {
    // Assignment
    { "=",              (enum ZenitTokenType[]){ T(ASSIGN), T(EOF) } },
    { "&",              (enum ZenitTokenType[]){ T(AMPERSAND), T(EOF) } },
};

void zenit_test_lexer_operators(void)
{
    zenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
