#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

void cenit_test_lexer_run(struct CenitLexerTest *tests, size_t count)
{
    for (size_t i=0; i < count; i++)
    {
        struct CenitLexerTest test = tests[i];

        CenitSourceInfo *srcinfo = cenit_source_new(CENIT_SOURCE_STRING, test.source);
        CenitLexer lexer = cenit_lexer_new(srcinfo);
        CenitToken *tokens = cenit_lexer_tokenize(&lexer);

        fl_expect("Tokenization must return a valid array of tokens", tokens != NULL);

        for (size_t j=0; test.tokens[j] != CENIT_TOKEN_EOF; j++)
            fl_vexpect(test.tokens[j] == tokens[j].type, "\"%s\": expected token at position %zu to be %s", test.source, j, cenit_token_print(tokens[j].type));

        fl_array_free(tokens);
        cenit_source_free(srcinfo);
    }
}
