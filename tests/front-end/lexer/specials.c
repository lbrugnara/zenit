

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) ZENIT_TOKEN_##token

static struct ZenitLexerTest tests[] = {
    { "#",              (enum ZenitTokenType[]){ T(HASH), T(EOF) } },
};

void zenit_test_lexer_specials(void)
{
    zenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
