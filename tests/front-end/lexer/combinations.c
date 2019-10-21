#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) CENIT_TOKEN_##token

static struct CenitLexerTest tests[] = {
    // Combinations
    { "[ 1, 2, 3 ]",                (CenitTokenType[]){ T(LBRACKET), T(INTEGER), T(COMMA), T(INTEGER), T(COMMA), T(INTEGER), T(RBRACKET), T(EOF) } },
    { "var id = 2",                 (CenitTokenType[]){ T(VAR), T(ID), T(ASSIGN), T(INTEGER), T(EOF) } },
    { "var id : uint8 = 2;",        (CenitTokenType[]){ T(VAR), T(ID), T(COLON), T(ID), T(ASSIGN), T(INTEGER), T(SEMICOLON), T(EOF) } },    
    { "var id : [10] uint8 = 2;",   (CenitTokenType[]){ T(VAR), T(ID), T(COLON), T(LBRACKET), T(INTEGER), T(RBRACKET), T(ID), T(ASSIGN), T(INTEGER), T(SEMICOLON), T(EOF) } },    
};

void cenit_test_lexer_combinations(void)
{
    cenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
