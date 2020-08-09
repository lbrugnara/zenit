
#include <errno.h>
#include "lexer.h"
#include "parser.h"
#include "ast/ast.h"

/* Public API */
ZenitParser zenit_parser_new(ZenitSourceInfo *srcinfo)
{
    return (ZenitParser){
        .lexer = zenit_lexer_new(srcinfo)
    };
}

bool zenit_parser_has_input(ZenitParser *parser)
{
    return zenit_lexer_peek(&parser->lexer).type != ZENIT_TOKEN_EOF;
}

void zenit_parser_peek(ZenitParser *parser, ZenitToken *token)
{
    ZenitToken next_token = zenit_lexer_peek(&parser->lexer);
    
    if (token)
        memcpy(token, &next_token, sizeof(ZenitToken));
}

bool zenit_parser_next_is(ZenitParser *parser, ZenitTokenType token_type)
{
    return zenit_lexer_peek(&parser->lexer).type == token_type;
}

ZenitToken zenit_parser_consume(ZenitParser *parser)
{
    return zenit_lexer_consume(&parser->lexer);
}

bool zenit_parser_expects(ZenitParser *parser, ZenitTokenType type, ZenitToken *consumed_token)
{
    ZenitToken token;
    zenit_parser_peek(parser, &token);

    if (token.type != type)
        return false;

    if (consumed_token)
        memcpy(consumed_token, &token, sizeof(ZenitToken));

    zenit_parser_consume(parser);
    
    return true;
}

bool zenit_parser_consume_if(ZenitParser *parser, ZenitTokenType type)
{
    if (zenit_lexer_peek(&parser->lexer).type == type)
    {
        zenit_parser_consume(parser);
        return true;
    }

    return false;
}
