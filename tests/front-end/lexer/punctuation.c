#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) CENIT_TOKEN_##token

static struct CenitLexerTest tests[] = {
    // Punctuation
    { ";",              (CenitTokenType[]){ T(SEMICOLON), T(EOF) } },
    { ":",              (CenitTokenType[]){ T(COLON), T(EOF) } },
    { ",",              (CenitTokenType[]){ T(COMMA), T(EOF) } },
    { ",,",             (CenitTokenType[]){ T(COMMA), T(COMMA), T(EOF) } },
    { ";;",             (CenitTokenType[]){ T(SEMICOLON), T(SEMICOLON), T(EOF) } },
    { ";:;",            (CenitTokenType[]){ T(SEMICOLON), T(COLON), T(SEMICOLON), T(EOF) } },
    { "{",              (CenitTokenType[]){ T(LBRACE), T(EOF) } },
    { "}",              (CenitTokenType[]){ T(RBRACE), T(EOF) } },
    { "{{}}",           (CenitTokenType[]){ T(LBRACE), T(LBRACE), T(RBRACE), T(RBRACE), T(EOF) } },
    { "[",              (CenitTokenType[]){ T(LBRACKET), T(EOF) } },
    { "]",              (CenitTokenType[]){ T(RBRACKET), T(EOF) } },
    { "[[]]",           (CenitTokenType[]){ T(LBRACKET), T(LBRACKET), T(RBRACKET), T(RBRACKET), T(EOF) } },
};

void cenit_test_lexer_punctuation(void)
{
    cenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
