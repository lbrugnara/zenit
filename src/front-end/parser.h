#ifndef CENIT_PARSER_H
#define CENIT_PARSER_H

#include "lexer.h"
#include "ast.h"

typedef enum CenitParserErrorType {
    // Cenit specific errors
    CENIT_ERROR_INTERNAL,

    // Mostly missing/unexpected tokens
    CENIT_ERROR_SYNTAX,

    // Number cannot be represented by Cenit
    CENIT_ERROR_LARGE_INTEGER
} CenitParserErrorType;

/*
 * Type: CenitParserError
 *  Represents an error in the parser
 */
typedef struct CenitParserError {
    const char *message;
    CenitParserErrorType type;
    unsigned int line;
    unsigned int col;
} CenitParserError;

/*
 * Type: CenitParser
 *  Object that keeps track of the parsing process
 */
typedef struct CenitParser {
    CenitLexer lexer;
    CenitParserError *errors;
} CenitParser;

/*
 * Function: cenit_parser_new
 *  Returns a parser object ready to parse the provided source. The user
 *  is expected to call <cenit_parser_free> to free the memory this
 *  object might use.
 *
 * Parameters:
 *  source - Source code to parse
 *
 * Returns:
 *  CenitParser - Parser object
 *
 */
CenitParser cenit_parser_new(const char *source);

/*
 * Function: cenit_parser_free
 *  Frees the memory used by the parser.
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 *  void - This function does not return a value
 */
void cenit_parser_free(CenitParser *parser);

/*
 * Function: cenit_parser_peek
 *  Peeks without consuming the next available token
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 *  CenitToken - The next available token
 *
 */
CenitToken cenit_parser_peek(CenitParser *parser);

/*
 * Function: cenit_parser_consume
 *  Consumes the next available token
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 *  CenitToken - The consumed token
 *
 */
CenitToken cenit_parser_consume(CenitParser *parser);

/*
 * Function: cenit_parser_expects
 *  If the type of the next available token matches with the provided <CenitTokenType>
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
bool cenit_parser_expects(CenitParser *parser, CenitTokenType type, CenitToken *consumed_token);


/*
 * Function: cenit_parser_has_input
 *  Returns *true* if there is pending input to process by the parser
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 *  bool - *true* if there is pending input, otherwise *false*.
 *
 */
bool cenit_parser_has_input(CenitParser *parser);

/*
 * Function: cenit_parser_parse
 *  Parses the whole source code and returns a <CenitAst> object.
 *  The returned object must be freed with the <cenit_ast_free>
 *  function.
 *  To know if the parsing succeeded or not the user needs to
 *  check the *errors* property in the <CenitParser> object
 *  after calling this function to know if the process populated
 *  it.
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 *  CenitAst* - Object that contains the parsed program
 *
 */
CenitAst* cenit_parser_parse(CenitParser *parser);

#endif /* CENIT_PARSER_H */
