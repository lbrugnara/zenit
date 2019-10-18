#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) CENIT_TOKEN_##token

static struct CenitLexerTest tests[] = {
    // Types
    { "1",              false,   (CenitTokenType[]){ T(INTEGER), T(EOF) } },
    { "1234",           false,   (CenitTokenType[]){ T(INTEGER), T(EOF) } },
    { "01234",          false,   (CenitTokenType[]){ T(INTEGER), T(EOF) } },
    { "4294967295",     false,   (CenitTokenType[]){ T(INTEGER), T(EOF) } },
};

void cenit_test_lexer_types(void)
{
    cenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
