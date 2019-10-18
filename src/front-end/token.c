#include "token.h"

static const char *TokenTypeString[] = {
    [CENIT_TOKEN_UNKNOWN]       = "UNKNOWN",
    
    [CENIT_TOKEN_INTEGER]       = "INTEGER",

    [CENIT_TOKEN_ID]            = "IDENTIFIER",
    [CENIT_TOKEN_VAR]            = "VAR (var)",

    [CENIT_TOKEN_ASSIGNMENT]    = "ASSIGNMENT (=)",

    [CENIT_TOKEN_COMMA]         = "COMMA (,)",
    [CENIT_TOKEN_SEMICOLON]     = "SEMICOLON (;)",
    [CENIT_TOKEN_COLON]         = "COLON (:)",
    [CENIT_TOKEN_LBRACE]        = "LEFT BRACE ({)",
    [CENIT_TOKEN_RBRACE]        = "RIGHT BRACE (})",
    [CENIT_TOKEN_LBRACKET]      = "LEFT BRACKET ([)",
    [CENIT_TOKEN_RBRACKET]      = "RIGHT BRACKET (])",

    [CENIT_TOKEN_EOF]           = "EOF",
};

const char* cenit_token_print(CenitTokenType type)
{
    if (type < 0 || type > CENIT_TOKEN_EOF)
        return "<invalid token>";

    return TokenTypeString[type];
}
