#ifndef ZENIT_TOKEN_H
#define ZENIT_TOKEN_H

#include <fllib.h>
#include "source.h"

/*
 * Enum: enum ZenitTokenType 
 *  The set of Zenit's token types
 */
enum ZenitTokenType {
    ZENIT_TOKEN_UNKNOWN,    

    // Types
    ZENIT_TOKEN_INTEGER,

    // Keywords
    ZENIT_TOKEN_ID,
    ZENIT_TOKEN_VAR,

    // Operators
    ZENIT_TOKEN_AMPERSAND,
    ZENIT_TOKEN_ASSIGN,

    // Punctuation
    ZENIT_TOKEN_COMMA,
    ZENIT_TOKEN_SEMICOLON,
    ZENIT_TOKEN_COLON,
    ZENIT_TOKEN_LBRACE,
    ZENIT_TOKEN_RBRACE,
    ZENIT_TOKEN_LBRACKET,
    ZENIT_TOKEN_RBRACKET,
    ZENIT_TOKEN_LPAREN,
    ZENIT_TOKEN_RPAREN,

    // Special tokens
    ZENIT_TOKEN_HASH,
    ZENIT_TOKEN_EOF,
};

/*
 * Struct: struct ZenitToken 
 *  Represents a token entity
 */
struct ZenitToken {
    enum ZenitTokenType type;
    struct FlSlice value;
    struct ZenitSourceLocation location;
};

/*
 * Function: zenit_token_print
 *  Returns a pointer to string that prints the friendly version
 *  of the enum <enum ZenitTokenType> value
 *
 * Parameters:
 *  type - enumeration value
 *
 * Returns:
 *  const char* - String pointer
 *
 */
const char* zenit_token_print(enum ZenitTokenType type);

#endif /* ZENIT_TOKEN_H */
