#ifndef ZENIT_TOKEN_H
#define ZENIT_TOKEN_H

#include <fllib/Slice.h>
#include "source.h"

/*
 * Enum: ZenitTokenType 
 *  The set of Zenit's token types
 */
typedef enum ZenitTokenType {
    ZENIT_TOKEN_UNKNOWN,    

    // Types
    ZENIT_TOKEN_INTEGER,
    ZENIT_TOKEN_BOOL,

    // Keywords
    ZENIT_TOKEN_ID,
    ZENIT_TOKEN_VAR,
    ZENIT_TOKEN_STRUCT,
    ZENIT_TOKEN_CAST,
    ZENIT_TOKEN_IF,
    ZENIT_TOKEN_ELSE,

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
} ZenitTokenType;

/*
 * Struct: ZenitToken 
 *  Represents a token entity
 */
typedef struct ZenitToken {
    ZenitTokenType type;
    struct FlSlice value;
    ZenitSourceLocation location;
} ZenitToken;

/*
 * Function: zenit_token_print
 *  Returns a pointer to string that prints the friendly version
 *  of the enum <ZenitTokenType> value
 *
 * Parameters:
 *  <ZenitTokenType> type: enumeration value
 *
 * Returns:
 *  <const char>*: String pointer
 *
 * Notes:
 *  The pointer returned by this function does not need to be
 *  freed
 */
const char* zenit_token_print(ZenitTokenType type);

#endif /* ZENIT_TOKEN_H */
