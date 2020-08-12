

#include <flut/flut.h>
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) ZENIT_TOKEN_##token

static struct ZenitLexerTest tests[] = {
    { "//",                                         (ZenitTokenType[]){ T(EOF) } },
    { "/**/",                                       (ZenitTokenType[]){ T(EOF) } },
    { "/* /* */ */",                                (ZenitTokenType[]){ T(EOF) } },
    { "var id = 1; // var id2 = 2;",                (ZenitTokenType[]){ T(VAR), T(ID), T(ASSIGN), T(INTEGER), T(EOF) } },
    { "var id = /* [ 1, 2, 3 ] */ 1;",              (ZenitTokenType[]){ T(VAR), T(ID), T(ASSIGN), T(INTEGER), T(EOF) } },
    { "var id = /* 1 /* [ 1, 2, 3 ] */ 2 */ 1;",    (ZenitTokenType[]){ T(VAR), T(ID), T(ASSIGN), T(INTEGER), T(EOF) } },
};

void zenit_test_lexer_comments(void)
{
    zenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
