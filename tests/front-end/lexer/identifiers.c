#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) CENIT_TOKEN_##token

static struct CenitLexerTest tests[] = {
    // Identifiers
    { "id",             false,   (CenitTokenType[]){ T(ID), T(EOF) } },
    { "name",           false,   (CenitTokenType[]){ T(ID), T(EOF) } },
    { "identifier",     false,   (CenitTokenType[]){ T(ID), T(EOF) } },
    { "something",      false,   (CenitTokenType[]){ T(ID), T(EOF) } },
    { "id123",          false,   (CenitTokenType[]){ T(ID), T(EOF) } },
    { "name_id",        false,   (CenitTokenType[]){ T(ID), T(EOF) } },
    { "int8",           false,   (CenitTokenType[]){ T(ID), T(EOF) } },
    { "int32",          false,   (CenitTokenType[]){ T(ID), T(EOF) } },
};

void cenit_test_lexer_identifiers(void)
{
    cenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
