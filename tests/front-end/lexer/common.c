#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

void cenit_test_lexer_run(struct CenitLexerTest *tests, size_t count)
{
    for (size_t i=0; i < count; i++)
    {
        struct CenitLexerTest test = tests[i];
        CenitLexer lexer = cenit_lexer_new(test.source);
        CenitToken *tokens = cenit_lexer_tokenize(&lexer);

        fl_expect("Tokenization must return a valid array of tokens", tokens != NULL);

        fl_vexpect(test.has_errors == lexer.has_errors, (test.has_errors ? "\"%s\" must contain errors" : "\"%s\" must not contain errors"), test.source);

        for (size_t j=0; test.tokens[j] != CENIT_TOKEN_EOF; j++)
            fl_vexpect(test.tokens[j] == tokens[j].type, "\"%s\": expected token at position %zu to be %s", test.source, j, cenit_token_print(tokens[j].type));

        fl_array_free(tokens);
    }
}
