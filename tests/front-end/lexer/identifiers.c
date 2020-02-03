

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) ZENIT_TOKEN_##token

static struct ZenitLexerTest tests[] = {
    // Identifiers
    { "id",             (enum ZenitTokenType[]){ T(ID), T(EOF) } },
    { "name",           (enum ZenitTokenType[]){ T(ID), T(EOF) } },
    { "identifier",     (enum ZenitTokenType[]){ T(ID), T(EOF) } },
    { "something",      (enum ZenitTokenType[]){ T(ID), T(EOF) } },
    { "id123",          (enum ZenitTokenType[]){ T(ID), T(EOF) } },
    { "name_id",        (enum ZenitTokenType[]){ T(ID), T(EOF) } },
    { "int8",           (enum ZenitTokenType[]){ T(ID), T(EOF) } },
    { "int32",          (enum ZenitTokenType[]){ T(ID), T(EOF) } },
};

void zenit_test_lexer_identifiers(void)
{
    zenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
