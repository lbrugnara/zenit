#ifndef CENIT_LEXER_H
#define CENIT_LEXER_H

#include <fllib.h>
#include "token.h"

/*
 * Struct: CenitLexer
 *  Object that keeps track of the lexical analysis phase
 *
 * Members:
 *  CenitSourceInfo *srcinfo - Object that contains the program's source code
 *  unsigned int index - Used as a pointer to perform the scan's operations
 */
typedef struct CenitLexer {
    CenitSourceInfo *srcinfo;
    unsigned int index;
} CenitLexer;

/*
 * Function: cenit_lexer_new
 *  Creates a new <CenitLexer> feeded with the provided source object.
 *
 * Parameters:
 *  srcinfo - Object that keeps track of source information
 *
 * Returns:
 *  CenitLexer - Object to keep track of the lexical analysis
 *
 */
CenitLexer cenit_lexer_new(CenitSourceInfo *srcinfo);

/*
 * Function: cenit_lexer_consume
 *  Consumes and returns the next available token.
 *
 * Parameters:
 *  lexer - Lexer object
 *
 * Returns:
 *  CenitToken - Consumed token
 *
 */
CenitToken cenit_lexer_consume(CenitLexer *lexer);

/*
 * Function: cenit_lexer_peek
 *  Returns the next available token without actually consuming it.
 *
 * Parameters:
 *  lexer - Lexer object
 *
 * Returns:
 *  CenitToken - Next available token to consume
 *
 */
CenitToken cenit_lexer_peek(CenitLexer *lexer);

/*
 * Function: cenit_lexer_tokenize
 *  Tokenizes the whole source content and returns an array
 *  containing all the tokens. The array must be freed with the
 *  fllib's <fl_array_free> function 
 *
 * Parameters:
 *  lexer - Object that makes the lexical analysis
 *
 * Returns:
 *  CenitToken* - Array of tokens. Must be freed with the <fl_array_free> function
 *
 */
CenitToken* cenit_lexer_tokenize(CenitLexer *lexer);

#endif /* CENIT_LEXER_H */
