#include "token.h"

static const char *TokenTypeString[] = {
    [ZENIT_TOKEN_UNKNOWN]       = "UNKNOWN",
    
    [ZENIT_TOKEN_INTEGER]       = "INTEGER",

    [ZENIT_TOKEN_ID]            = "IDENTIFIER",
    [ZENIT_TOKEN_VAR]           = "VAR \"var\"",

    [ZENIT_TOKEN_AMPERSAND]     = "AMPERSAND \"&\"",
    [ZENIT_TOKEN_ASSIGN]        = "ASSIGN \"=\"",

    [ZENIT_TOKEN_COMMA]         = "COMMA \",\"",
    [ZENIT_TOKEN_SEMICOLON]     = "SEMICOLON \";\"",
    [ZENIT_TOKEN_COLON]         = "COLON \":\"",
    [ZENIT_TOKEN_LBRACE]        = "LEFT BRACE \"{\"",
    [ZENIT_TOKEN_RBRACE]        = "RIGHT BRACE \"}\"",
    [ZENIT_TOKEN_LBRACKET]      = "LEFT BRACKET \"[\"",
    [ZENIT_TOKEN_RBRACKET]      = "RIGHT BRACKET \"]\"",
    [ZENIT_TOKEN_LPAREN]        = "LEFT \"PARENTHESIS\"",
    [ZENIT_TOKEN_RPAREN]        = "RIGHT PARENTHESIS \")\"",

    [ZENIT_TOKEN_HASH]          = "HASH \"#\"",
    [ZENIT_TOKEN_EOF]           = "EOF",
};

const char* zenit_token_print(enum ZenitTokenType type)
{
    if (type < 0 || type > ZENIT_TOKEN_EOF)
        return "<invalid token>";

    return TokenTypeString[type];
}
