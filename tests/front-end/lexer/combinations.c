#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) ZENIT_TOKEN_##token

static struct ZenitLexerTest tests[] = {
    // Combinations
    { "[ 1, 2, 3 ]",                        (enum ZenitTokenType[]){ T(LBRACKET), T(INTEGER), T(COMMA), T(INTEGER), T(COMMA), T(INTEGER), T(RBRACKET), T(EOF) } },
    { "var id = 2",                         (enum ZenitTokenType[]){ T(VAR), T(ID), T(ASSIGN), T(INTEGER), T(EOF) } },
    { "var id : uint8 = 2;",                (enum ZenitTokenType[]){ T(VAR), T(ID), T(COLON), T(ID), T(ASSIGN), T(INTEGER), T(SEMICOLON), T(EOF) } },
    { "var id : [10] uint8 = 2;",           (enum ZenitTokenType[]){ T(VAR), T(ID), T(COLON), T(LBRACKET), T(INTEGER), T(RBRACKET), T(ID), T(ASSIGN), T(INTEGER), T(SEMICOLON), T(EOF) } },
    { "var idref = &id;",                   (enum ZenitTokenType[]){ T(VAR), T(ID), T(ASSIGN), T(AMPERSAND), T(ID), T(SEMICOLON), T(EOF) } },
    { "var idref2 : &uint8 = &id;",         (enum ZenitTokenType[]){ T(VAR), T(ID), T(COLON), T(AMPERSAND), T(ID), T(ASSIGN), T(AMPERSAND), T(ID), T(SEMICOLON), T(EOF) } },
    { "#[attr]",                            (enum ZenitTokenType[]){ T(HASH), T(LBRACKET), T(ID), T(RBRACKET), T(EOF) } },
    { "#[attr()]",                          (enum ZenitTokenType[]){ T(HASH), T(LBRACKET), T(ID), T(LPAREN), T(RPAREN), T(RBRACKET), T(EOF) } },
    { "#[attr(key: value)]",                (enum ZenitTokenType[]){ T(HASH), T(LBRACKET), T(ID), T(LPAREN), T(ID), T(COLON), T(ID), T(RPAREN), T(RBRACKET), T(EOF) } },
    { "#[attr(key: value, key2: value2)]",  (enum ZenitTokenType[]){ T(HASH), T(LBRACKET), T(ID), T(LPAREN), T(ID), T(COLON), T(ID), T(COMMA), T(ID), T(COLON), T(ID), T(RPAREN), T(RBRACKET), T(EOF) } },
};

void zenit_test_lexer_combinations(void)
{
    zenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
