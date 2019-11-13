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

struct ZenitToken zenit_parser_peek(struct ZenitParser *parser)
{
    return zenit_lexer_peek(&parser->lexer);
}

struct ZenitToken zenit_parser_consume(struct ZenitParser *parser)
{
    return zenit_lexer_consume(&parser->lexer);
}

bool zenit_parser_expects(struct ZenitParser *parser, enum ZenitTokenType type, struct ZenitToken *consumed_token)
{
    struct ZenitToken token = zenit_parser_peek(parser);

    if (token.type != type)
        return false;

    if (consumed_token)
        memcpy(consumed_token, &token, sizeof(struct ZenitToken));

    zenit_parser_consume(parser);
    
    return true;
}

bool zenit_parser_consume_if(struct ZenitParser *parser, enum ZenitTokenType type)
{
    if (zenit_parser_peek(parser).type == type)
    {
        zenit_parser_consume(parser);
        return true;
    }

    return false;
}
