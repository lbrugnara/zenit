#ifndef ZENIT_PARSER_H
#define ZENIT_PARSER_H

#include "context.h"
#include "lexer.h"
#include "ast/ast.h"

/*
 * Struct: ZenitParser
 *  Object to keep track of the parsing process
 */
typedef struct ZenitParser {
    ZenitLexer lexer;
} ZenitParser;

/*
 * Function: zenit_parser_new
 *  Returns a parser object ready to parse the provided source.
 *
 * Parameters:
 *  <ZenitSourceInfo> *srcinfo: Object that keeps track of the source information
 *
 * Returns:
 *  <ZenitParser>: Parser object
 *
 */
ZenitParser zenit_parser_new(ZenitSourceInfo *srcinfo);

/*
 * Function: zenit_parser_peek
 *  Peeks without consuming the next available token
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitToken> *token: If not NULL the peeked token is copied to it
 *
 * Returns:
 *  <void>: This function does not return a value
 *
 */
void zenit_parser_peek(ZenitParser *parser, ZenitToken *token);

/*
 * Function: zenit_parser_next_is
 *  Returns if the next available token matches with the provided token type
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitTokenType> token_type: Expected token type
 * 
 * Returns:
 *  <bool>: *true* if the token type matches, otherwise this function returns *false*.
 * 
 */
bool zenit_parser_next_is(ZenitParser *parser, ZenitTokenType token_type);

/*
 * Function: zenit_parser_consume
 *  Consumes the next available token
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *
 * Returns:
 *  <ZenitToken>: The consumed token
 *
 */
ZenitToken zenit_parser_consume(ZenitParser *parser);

/*
 * Function: zenit_parser_expects
 *  If the type of the next available token matches with the provided <ZenitTokenType>
 *  the parser consumes the token, if *consumed_token* is not NULL copies the consumed token
 *  to it, and returns *true*. If the type does not match this function returns *false* without
 *  consuming anything.
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitTokenType> type: Expected token type
 *  <ZenitToken> *consumed_token: If not NULL and the types match, the consumed token will be copied to it
 *
 * Returns:
 *  <bool>: *true* if the next token's type matches the provided type.
 *
 */
bool zenit_parser_expects(ZenitParser *parser, ZenitTokenType type, ZenitToken *consumed_token);

/*
 * Function: zenit_parser_consume_if
 *  If the type of the following token is equals to the provided *type*
 *  this function consumes the token and returns true. If the types differ, the parser
 *  remains in the same position and returns false
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitTokenType> type: Expected type of the following token
 *
 * Returns:
 *  <bool>: *true* if the following token matches the *type*, otherwise *false*.
 */
bool zenit_parser_consume_if(ZenitParser *parser, ZenitTokenType type);

/*
 * Function: zenit_parser_has_input
 *  Returns *true* if there is pending input to process by the parser
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *
 * Returns:
 *  <bool>: *true* if there is pending input, otherwise *false*.
 *
 */
bool zenit_parser_has_input(ZenitParser *parser);

#endif /* ZENIT_PARSER_H */
