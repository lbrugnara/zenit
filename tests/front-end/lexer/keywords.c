#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) CENIT_TOKEN_##token

static struct CenitLexerTest tests[] = {
    // Keywords
    { "var",            false,   (CenitTokenType[]){ T(VAR), T(EOF) } },
};

void cenit_test_lexer_keywords(void)
{
    cenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
