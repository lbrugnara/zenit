#ifndef CENIT_TESTS_LEXER_H
#define CENIT_TESTS_LEXER_H

#include "../../../src/front-end/token.h"

struct CenitLexerTest {
    const char *source;
    const bool has_errors;
    CenitTokenType *tokens;
};

void cenit_test_lexer_run(struct CenitLexerTest *tests, size_t count);

// Tests
void cenit_test_lexer_types(void);
void cenit_test_lexer_assignment(void);
void cenit_test_lexer_identifiers(void);
void cenit_test_lexer_keywords(void);
void cenit_test_lexer_punctuation(void);
void cenit_test_lexer_errors(void);
void cenit_test_lexer_combinations(void);

#endif /* CENIT_TESTS_LEXER_H */
