

#include <flut/flut.h>
#include "../../../src/front-end/lexer.h"
#include "tests.h"

#define T(token) ZENIT_TOKEN_##token

static struct ZenitLexerTest tests[] = {
    // Identifiers
    { "id",             (ZenitTokenType[]){ T(ID), T(EOF) } },
    { "name",           (ZenitTokenType[]){ T(ID), T(EOF) } },
    { "identifier",     (ZenitTokenType[]){ T(ID), T(EOF) } },
    { "something",      (ZenitTokenType[]){ T(ID), T(EOF) } },
    { "id123",          (ZenitTokenType[]){ T(ID), T(EOF) } },
    { "name_id",        (ZenitTokenType[]){ T(ID), T(EOF) } },
    { "int8",           (ZenitTokenType[]){ T(ID), T(EOF) } },
    { "int32",          (ZenitTokenType[]){ T(ID), T(EOF) } },
};

void zenit_test_lexer_identifiers(void)
{
    zenit_test_lexer_run(tests, sizeof(tests) / sizeof(tests[0]));
}
