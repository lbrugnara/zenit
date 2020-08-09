

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) ZENIT_TOKEN_##token

static struct ZenitLexerTest tests[] = {
    // Keywords
    { "var",            (ZenitTokenType[]){ T(VAR), T(EOF) }       },
    { "cast",           (ZenitTokenType[]){ T(CAST), T(EOF) }      },
    { "struct",         (ZenitTokenType[]){ T(STRUCT), T(EOF) }    },
    { "if",             (ZenitTokenType[]){ T(IF), T(EOF) }        },
};

void zenit_test_lexer_keywords(void)
{
    zenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
