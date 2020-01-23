#ifndef ZENIT_TESTS_LEXER_H
#define ZENIT_TESTS_LEXER_H

#include "../../../src/front-end/token.h"

struct ZenitLexerTest {
    const char *source;
    enum ZenitTokenType *tokens;
};

void zenit_test_lexer_run(struct ZenitLexerTest *tests, size_t count);

// Tests
void zenit_test_lexer_types(void);
void zenit_test_lexer_operators(void);
void zenit_test_lexer_identifiers(void);
void zenit_test_lexer_keywords(void);
void zenit_test_lexer_punctuation(void);
void zenit_test_lexer_specials(void);
void zenit_test_lexer_errors(void);
void zenit_test_lexer_combinations(void);
void zenit_test_lexer_comments(void);

#endif /* ZENIT_TESTS_LEXER_H */
