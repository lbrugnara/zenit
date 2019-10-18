#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) CENIT_TOKEN_##token

static struct CenitLexerTest tests[] = {
    // Punctuation
    { ";",              false,   (CenitTokenType[]){ T(SEMICOLON), T(EOF) } },
    { ":",              false,   (CenitTokenType[]){ T(COLON), T(EOF) } },
    { ",",              false,   (CenitTokenType[]){ T(COMMA), T(EOF) } },
    { ",,",              false,   (CenitTokenType[]){ T(COMMA), T(COMMA), T(EOF) } },
    { ";;",             false,   (CenitTokenType[]){ T(SEMICOLON), T(SEMICOLON), T(EOF) } },
    { ";:;",            false,   (CenitTokenType[]){ T(SEMICOLON), T(COLON), T(SEMICOLON), T(EOF) } },
    { "{",              false,   (CenitTokenType[]){ T(LBRACE), T(EOF) } },
    { "}",              false,   (CenitTokenType[]){ T(RBRACE), T(EOF) } },
    { "{{}}",           false,   (CenitTokenType[]){ T(LBRACE), T(LBRACE), T(RBRACE), T(RBRACE), T(EOF) } },
    { "[",              false,   (CenitTokenType[]){ T(LBRACKET), T(EOF) } },
    { "]",              false,   (CenitTokenType[]){ T(RBRACKET), T(EOF) } },
    { "[[]]",           false,   (CenitTokenType[]){ T(LBRACKET), T(LBRACKET), T(RBRACKET), T(RBRACKET), T(EOF) } },
};

void cenit_test_lexer_punctuation(void)
{
    cenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
