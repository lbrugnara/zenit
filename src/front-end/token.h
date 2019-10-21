#ifndef CENIT_TOKEN_H
#define CENIT_TOKEN_H

#include <fllib.h>
#include "source.h"

/*
 * Enum: CenitTokenType 
 *  The set of Cenit's token types
 */
typedef enum CenitTokenType {
    CENIT_TOKEN_UNKNOWN,    

    // Types
    CENIT_TOKEN_INTEGER,

    // Keywords
    CENIT_TOKEN_ID,
    CENIT_TOKEN_VAR,

    // Assignment
    CENIT_TOKEN_ASSIGN,

    // Punctuation
    CENIT_TOKEN_COMMA,
    CENIT_TOKEN_SEMICOLON,
    CENIT_TOKEN_COLON,
    CENIT_TOKEN_LBRACE,
    CENIT_TOKEN_RBRACE,
    CENIT_TOKEN_LBRACKET,
    CENIT_TOKEN_RBRACKET,

    // Special tokens
    CENIT_TOKEN_EOF,
} CenitTokenType;

/*
 * Struct: CenitToken 
 *  Represents a token entity
 */
typedef struct CenitToken {
    CenitTokenType type;
    struct FlSlice value;
    CenitSourceLocation location;
} CenitToken;

/*
 * Function: cenit_token_print
 *  Returns a pointer to string that prints the friendly version
 *  of the enum <CenitTokenType> value
 *
 * Parameters:
 *  type - enumeration value
 *
 * Returns:
 *  const char* - String pointer
 *
 */
const char* cenit_token_print(CenitTokenType type);

#endif /* CENIT_TOKEN_H */
