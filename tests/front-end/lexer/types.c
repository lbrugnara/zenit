

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) ZENIT_TOKEN_##token

static struct ZenitLexerTest tests[] = {
    // Types
    { "1",              (enum ZenitTokenType[]){ T(INTEGER), T(EOF) } },
    { "1234",           (enum ZenitTokenType[]){ T(INTEGER), T(EOF) } },
    { "01234",          (enum ZenitTokenType[]){ T(INTEGER), T(EOF) } },
    { "4294967295",     (enum ZenitTokenType[]){ T(INTEGER), T(EOF) } },
    { "0xA",            (enum ZenitTokenType[]){ T(INTEGER), T(EOF) } },
    { "0xa",            (enum ZenitTokenType[]){ T(INTEGER), T(EOF) } },
    { "0xFF",           (enum ZenitTokenType[]){ T(INTEGER), T(EOF) } },
};

void zenit_test_lexer_types(void)
{
    zenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
