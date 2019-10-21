#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) CENIT_TOKEN_##token

static struct CenitLexerTest tests[] = {
    // Identifiers
    { "id",             (CenitTokenType[]){ T(ID), T(EOF) } },
    { "name",           (CenitTokenType[]){ T(ID), T(EOF) } },
    { "identifier",     (CenitTokenType[]){ T(ID), T(EOF) } },
    { "something",      (CenitTokenType[]){ T(ID), T(EOF) } },
    { "id123",          (CenitTokenType[]){ T(ID), T(EOF) } },
    { "name_id",        (CenitTokenType[]){ T(ID), T(EOF) } },
    { "int8",           (CenitTokenType[]){ T(ID), T(EOF) } },
    { "int32",          (CenitTokenType[]){ T(ID), T(EOF) } },
};

void cenit_test_lexer_identifiers(void)
{
    cenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
