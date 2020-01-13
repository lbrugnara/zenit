#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) ZENIT_TOKEN_##token

static struct ZenitLexerTest tests[] = {
    // Keywords
    { "var",            (enum ZenitTokenType[]){ T(VAR), T(EOF) }       },
    { "cast",           (enum ZenitTokenType[]){ T(CAST), T(EOF) }      },
    { "struct",         (enum ZenitTokenType[]){ T(STRUCT), T(EOF) }    },
};

void zenit_test_lexer_keywords(void)
{
    zenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
