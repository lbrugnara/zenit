#include <fllib.h>
#include <errno.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

/* Public API */
struct ZenitParser zenit_parser_new(struct ZenitSourceInfo *srcinfo)
{
    return (struct ZenitParser){
        .lexer = zenit_lexer_new(srcinfo)
    };
}

bool zenit_parser_has_input(struct ZenitParser *parser)
{
    return zenit_lexer_peek(&parser->lexer).type != ZENIT_TOKEN_EOF;
}

void zenit_parser_peek(struct ZenitParser *parser, struct ZenitToken *token)
{
    struct ZenitToken next_token = zenit_lexer_peek(&parser->lexer);
    
    if (token)
        memcpy(token, &next_token, sizeof(struct ZenitToken));
}

bool zenit_parser_next_is(struct ZenitParser *parser, enum ZenitTokenType token_type)
{
    return zenit_lexer_peek(&parser->lexer).type == token_type;
}

struct ZenitToken zenit_parser_consume(struct ZenitParser *parser)
{
    return zenit_lexer_consume(&parser->lexer);
}

bool zenit_parser_expects(struct ZenitParser *parser, enum ZenitTokenType type, struct ZenitToken *consumed_token)
{
    struct ZenitToken token;
    zenit_parser_peek(parser, &token);

    if (token.type != type)
        return false;

    if (consumed_token)
        memcpy(consumed_token, &token, sizeof(struct ZenitToken));

    zenit_parser_consume(parser);
    
    return true;
}

bool zenit_parser_consume_if(struct ZenitParser *parser, enum ZenitTokenType type)
{
    if (zenit_lexer_peek(&parser->lexer).type == type)
    {
        zenit_parser_consume(parser);
        return true;
    }

    return false;
}
