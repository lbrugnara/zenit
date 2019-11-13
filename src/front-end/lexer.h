#ifndef ZENIT_LEXER_H
#define ZENIT_LEXER_H

#include <fllib.h>
#include "token.h"

/*
 * Struct: struct ZenitLexer
 *  Object that keeps track of the lexical analysis phase
 *
 * Members:
 *  struct ZenitSourceInfo *srcinfo - Object that contains the program's source code
 *  unsigned int index - Used as a pointer to perform the scan's operations
 */
struct ZenitLexer {
    struct ZenitSourceInfo *srcinfo;
    unsigned int index;
};

/*
 * Function: zenit_lexer_new
 *  Creates a new <struct ZenitLexer> feeded with the provided source object.
 *
 * Parameters:
 *  srcinfo - Object that keeps track of source information
 *
 * Returns:
 *  struct ZenitLexer - Object to keep track of the lexical analysis
 *
 */
struct ZenitLexer zenit_lexer_new(struct ZenitSourceInfo *srcinfo);

/*
 * Function: zenit_lexer_consume
 *  Consumes and returns the next available token.
 *
 * Parameters:
 *  lexer - Lexer object
 *
 * Returns:
 *  struct ZenitToken - Consumed token
 *
 */
struct ZenitToken zenit_lexer_consume(struct ZenitLexer *lexer);

/*
 * Function: zenit_lexer_peek
 *  Returns the next available token without actually consuming it.
 *
 * Parameters:
 *  lexer - Lexer object
 *
 * Returns:
 *  struct ZenitToken - Next available token to consume
 *
 */
struct ZenitToken zenit_lexer_peek(struct ZenitLexer *lexer);

/*
 * Function: zenit_lexer_tokenize
 *  Tokenizes the whole source content and returns an array
 *  containing all the tokens. The array must be freed with the
 *  fllib's <fl_array_free> function 
 *
 * Parameters:
 *  lexer - Object that makes the lexical analysis
 *
 * Returns:
 *  struct ZenitToken* - Array of tokens. Must be freed with the <fl_array_free> function
 *
 */
struct ZenitToken* zenit_lexer_tokenize(struct ZenitLexer *lexer);

#endif /* ZENIT_LEXER_H */
