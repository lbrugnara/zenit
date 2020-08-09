
#include <fllib/Array.h>
#include "../../Test.h"
#include "../../../src/front-end/lexer.h"
#include "tests.h"

void zenit_test_lexer_run(struct ZenitLexerTest *tests, size_t count)
{
    for (size_t i=0; i < count; i++)
    {
        struct ZenitLexerTest test = tests[i];

        ZenitSourceInfo *srcinfo = zenit_source_new(ZENIT_SOURCE_STRING, test.source);
        ZenitLexer lexer = zenit_lexer_new(srcinfo);
        ZenitToken *tokens = zenit_lexer_tokenize(&lexer);

        fl_expect("Tokenization must return a valid array of tokens", tokens != NULL);

        for (size_t j=0; test.tokens[j] != ZENIT_TOKEN_EOF; j++)
            fl_vexpect(test.tokens[j] == tokens[j].type, "\"%s\": expected token at position %zu to be %s", test.source, j, zenit_token_print(tokens[j].type));

        fl_array_free(tokens);
        zenit_source_free(srcinfo);
    }
}
