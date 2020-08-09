

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) ZENIT_TOKEN_##token

static struct ZenitLexerTest tests[] = {
    // Types
    { "1",              (ZenitTokenType[]){ T(INTEGER), T(EOF) }   },
    { "1234",           (ZenitTokenType[]){ T(INTEGER), T(EOF) }   },
    { "01234",          (ZenitTokenType[]){ T(INTEGER), T(EOF) }   },
    { "4294967295",     (ZenitTokenType[]){ T(INTEGER), T(EOF) }   },
    { "0xA",            (ZenitTokenType[]){ T(INTEGER), T(EOF) }   },
    { "0xa",            (ZenitTokenType[]){ T(INTEGER), T(EOF) }   },
    { "0xFF",           (ZenitTokenType[]){ T(INTEGER), T(EOF) }   },
    { "true",           (ZenitTokenType[]){ T(BOOL), T(EOF) }      },
    { "false",          (ZenitTokenType[]){ T(BOOL), T(EOF) }      },
};

void zenit_test_lexer_types(void)
{
    zenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
