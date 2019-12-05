#ifndef ZENIT_PARSER_H
#define ZENIT_PARSER_H

#include "context.h"
#include "lexer.h"
#include "ast.h"

/*
 * Struct: struct ZenitParser
 *  Object to keep track of the parsing process
 */
struct ZenitParser {
    struct ZenitLexer lexer;
};

/*
 * Function: zenit_parser_new
 *  Returns a parser object ready to parse the provided source.
 *
 * Parameters:
 *  srcinfo - Object that keeps track of the source information
 *
 * Returns:
 *  struct ZenitParser - Parser object
 *
 */
struct ZenitParser zenit_parser_new(struct ZenitSourceInfo *srcinfo);

/*
 * Function: zenit_parser_peek
 *  Peeks without consuming the next available token
 *
 * Parameters:
 *  parser - Parser object
 *  token - If not NULL the peeked token is copied to it
 *
 * Returns:
 *  void - This function does not return a value
 *
 */
void zenit_parser_peek(struct ZenitParser *parser, struct ZenitToken *token);

/*
 * Function: zenit_parser_next_is
 *  Returns if the next available token matches with the provided token type
 *
 * Parameters:
 *  <struct ZenitParser> *parser - Parser object
 *  <enum ZenitTokenType> token_type: Expected token type
 * 
 * Returns:
 *  bool - *true* if the token type matches, otherwise this function returns *false*.
 * 
 */
bool zenit_parser_next_is(struct ZenitParser *parser, enum ZenitTokenType token_type);

/*
 * Function: zenit_parser_consume
 *  Consumes the next available token
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 *  struct ZenitToken - The consumed token
 *
 */
struct ZenitToken zenit_parser_consume(struct ZenitParser *parser);

/*
 * Function: zenit_parser_expects
 *  If the type of the next available token matches with the provided <enum ZenitTokenType>
 *  the parser consumes the token, if *consumed_token* is not NULL copies the consumed token
 *  to it, and returns *true*. If the type does not match this function returns *false* without
 *  consuming anything.
 *
 * Parameters:
 *  parser - Parser object
 *  type - Expected token type
 *  consumed_token - If not NULL and the types match, the consumed token will be copied to it
 *
 * Returns:
 *  bool - *true* if the next token's type matches the provided type.
 *
 */
bool zenit_parser_expects(struct ZenitParser *parser, enum ZenitTokenType type, struct ZenitToken *consumed_token);

/*
 * Function: zenit_parser_consume_if
 *  If the type of the following token is equals to the provided *type*
 *  this function consumes the token and returns true. If the types differ, the parser
 *  remains in the same position and returns false
 *
 * Parameters:
 *  parser - Parser object
 *  type - Expected type of the following token
 *
 * Returns:
 *  bool - *true* if the following token matches the *type*, otherwise *false*.
 */
bool zenit_parser_consume_if(struct ZenitParser *parser, enum ZenitTokenType type);

/*
 * Function: zenit_parser_has_input
 *  Returns *true* if there is pending input to process by the parser
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 *  bool - *true* if there is pending input, otherwise *false*.
 *
 */
bool zenit_parser_has_input(struct ZenitParser *parser);

#endif /* ZENIT_PARSER_H */
