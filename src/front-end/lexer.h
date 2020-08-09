#ifndef ZENIT_LEXER_H
#define ZENIT_LEXER_H

#include "token.h"

/*
 * Struct: ZenitLexer
 *  Object that keeps track of the lexical analysis phase
 *
 * Members:
 *  <ZenitSourceInfo> *srcinfo: Object that contains the program's source code
 *  <unsigned int> index: Used as a pointer to perform the scan's operations
 */
typedef struct ZenitLexer {
    ZenitSourceInfo *srcinfo;
    unsigned int index;
} ZenitLexer;

/*
 * Function: zenit_lexer_new
 *  Creates a new <ZenitLexer> feeded with the provided source object.
 *
 * Parameters:
 *  <ZenitSourceInfo> *srcinfo: Object that keeps track of source information
 *
 * Returns:
 *  <ZenitLexer>: Object to keep track of the lexical analysis
 *
 */
ZenitLexer zenit_lexer_new(ZenitSourceInfo *srcinfo);

/*
 * Function: zenit_lexer_consume
 *  Consumes and returns the next available token.
 *
 * Parameters:
 *  <ZenitLexer> *lexer: Lexer object
 *
 * Returns:
 *  <ZenitToken>: Consumed token
 *
 */
ZenitToken zenit_lexer_consume(ZenitLexer *lexer);

/*
 * Function: zenit_lexer_peek
 *  Returns the next available token without actually consuming it.
 *
 * Parameters:
 *  <ZenitLexer> *lexer: Lexer object
 *
 * Returns:
 *  <ZenitToken>: Next available token to consume
 *
 */
ZenitToken zenit_lexer_peek(ZenitLexer *lexer);

/*
 * Function: zenit_lexer_tokenize
 *  Tokenizes the whole source content and returns an array
 *  containing all the tokens. The array must be freed with the
 *  fllib's <fl_array_free> function 
 *
 * Parameters:
 *  <ZenitLexer> *lexer: Object that makes the lexical analysis
 *
 * Returns:
 *  <ZenitToken>*: Array of tokens. Must be freed with the <fl_array_free> function
 *
 */
ZenitToken* zenit_lexer_tokenize(ZenitLexer *lexer);

#endif /* ZENIT_LEXER_H */
