

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) ZENIT_TOKEN_##token

static struct ZenitLexerTest tests[] = {
    // Punctuation
    { ";",              (ZenitTokenType[]){ T(SEMICOLON), T(EOF) } },
    { ":",              (ZenitTokenType[]){ T(COLON), T(EOF) } },
    { ",",              (ZenitTokenType[]){ T(COMMA), T(EOF) } },
    { ",,",             (ZenitTokenType[]){ T(COMMA), T(COMMA), T(EOF) } },
    { ";;",             (ZenitTokenType[]){ T(SEMICOLON), T(SEMICOLON), T(EOF) } },
    { ";:;",            (ZenitTokenType[]){ T(SEMICOLON), T(COLON), T(SEMICOLON), T(EOF) } },
    { "{",              (ZenitTokenType[]){ T(LBRACE), T(EOF) } },
    { "}",              (ZenitTokenType[]){ T(RBRACE), T(EOF) } },
    { "{{}}",           (ZenitTokenType[]){ T(LBRACE), T(LBRACE), T(RBRACE), T(RBRACE), T(EOF) } },
    { "[",              (ZenitTokenType[]){ T(LBRACKET), T(EOF) } },
    { "]",              (ZenitTokenType[]){ T(RBRACKET), T(EOF) } },
    { "[[]]",           (ZenitTokenType[]){ T(LBRACKET), T(LBRACKET), T(RBRACKET), T(RBRACKET), T(EOF) } },
    { "(",              (ZenitTokenType[]){ T(LPAREN), T(EOF) } },
    { ")",              (ZenitTokenType[]){ T(RPAREN), T(EOF) } },
    { "(())",           (ZenitTokenType[]){ T(LPAREN), T(LPAREN), T(RPAREN), T(RPAREN), T(EOF) } },
};

void zenit_test_lexer_punctuation(void)
{
    zenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
