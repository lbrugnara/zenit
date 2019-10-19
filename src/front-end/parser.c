#include <fllib.h>
#include <errno.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

/* Public API */
CenitParser cenit_parser_new(const char *source)
{
    return (CenitParser){
        .lexer = cenit_lexer_new(source)
    };
}

bool cenit_parser_has_input(CenitParser *parser)
{
    return cenit_lexer_peek(&parser->lexer).type != CENIT_TOKEN_EOF;
}

CenitToken cenit_parser_peek(CenitParser *parser)
{
    return cenit_lexer_peek(&parser->lexer);
}

CenitToken cenit_parser_consume(CenitParser *parser)
{
    return cenit_lexer_consume(&parser->lexer);
}

bool cenit_parser_expects(CenitParser *parser, CenitTokenType type, CenitToken *consumed_token)
{
    CenitToken token = cenit_parser_peek(parser);

    if (token.type != type)
        return false;

    if (consumed_token)
        memcpy(consumed_token, &token, sizeof(CenitToken));

    cenit_parser_consume(parser);
    
    return true;
}
