#include <fllib.h>
#include <errno.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

/* Private API */
static CenitNode* parse_integer_literal(CenitParser *parser);
static CenitNode* parse_literal_expression(CenitParser *parser);
static CenitNode* parse_array_initializer(CenitParser *parser);
static CenitNode* parse_expression(CenitParser *parser);
static CenitNode* parse_variable_declaration(CenitParser *parser);
static CenitNode* parse_expression_statement(CenitParser *parser);
static CenitNode* parse_statement(CenitParser *parser);
static CenitNode* parse_declaration(CenitParser *parser);

/*
 * Function: parser_error
 *  Adds a new error to the parser object. If the *errors* property
 *  is NULL, this function allocates memory with the <fl_array_new> function.
 *
 * Parameters:
 *  parser - Parser object
 *  message - Error message
 *  type - Token type
 *  token_value - The token value
 *  line - Line number
 *  col - Column number
 *
 * Returns:
 *  void - This function does not return a value
 */
static void parser_error(CenitParser *parser, CenitParserErrorType type, const char *message, unsigned int line, unsigned int col)
{
    if (parser->errors == NULL)
        parser->errors = fl_array_new(sizeof(CenitParserError), 0);

    CenitParserError error = {
        .type = type,
        .line = line,
        .col = col,
        .message = fl_cstring_dup(message)
    };

    parser->errors = fl_array_append(parser->errors, &error);
}

/*
 * Function: error_free
 *  Frees the memory of an error object allocated with the <parser_error> function
 *
 * Parameters:
 *  errorptr - Pointer to a <CenitParserError> object
 *
 * Returns:
 *  void - This function does not return a value
 *
 */
static void error_free(void *errorptr)
{
    if (!errorptr)
        return;

    CenitParserError *error = (CenitParserError*)errorptr;

    if (error->message)
        fl_cstring_free(error->message);
}


/*
 * Function: parse_integer_literal
 *  Parses an integer literal
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 *  CenitNode* - The parsed integer literal
 *
 * Grammar:
 *  integer_literal = ( '+' | '-' )? INTEGER
 */
static CenitNode* parse_integer_literal(CenitParser *parser)
{
    CenitToken temp_token;

    // By now we just parse integers, particularly uint8
    if (!cenit_parser_expects(parser, CENIT_TOKEN_INTEGER, &temp_token))
    {
        parser_error(parser, CENIT_ERROR_SYNTAX, "Expecting an integer literal", parser->lexer.line, parser->lexer.col);
        return NULL;
    }

    // Create the literal node with the base information
    CenitLiteralNode *literal_node = fl_malloc(sizeof(CenitLiteralNode));
    literal_node->base.type = CENIT_NODE_LITERAL;
    literal_node->base.col = temp_token.col;
    literal_node->base.line = temp_token.line;

    // This is the integer parsing logic (unsigned integers in base 10 by now)
    char *endptr;
    unsigned long long temp_int = strtoull((const char*)temp_token.value.sequence, &endptr, 10);
    
    // The token length must be equals to the parsed number
    if ((void*)(temp_token.value.sequence + temp_token.value.length) != (void*)endptr)
        goto range_error;

    // Check for specific strtoull errors
    if ((temp_int == ULLONG_MAX && errno == ERANGE) || (temp_int == 0ULL && errno == EINVAL))
    {
        parser_error(parser, CENIT_ERROR_LARGE_INTEGER, "Integral value is too large", parser->lexer.line, parser->lexer.col);
        goto range_error;
    }

    // Check the type of the literal value
    if (temp_int <= UINT8_MAX)
    {
        literal_node->value.uint8 = (uint8_t)temp_int;
        literal_node->typeinfo.type = CENIT_TYPE_UINT8;
        literal_node->typeinfo.is_array = false;
        literal_node->typeinfo.elements = 1;
    }
    else
    {
        parser_error(parser, CENIT_ERROR_LARGE_INTEGER, "Integral value is too large", parser->lexer.line, parser->lexer.col);
        goto range_error;
    }

    // Success
    return (CenitNode*)literal_node;

    // Cleanup code for error conditions
    range_error:
        cenit_node_free((CenitNode*)literal_node);

    return NULL;
}

/*
 * Function: parse_literal_expression
 *  Parses all the literal expressions supported by the language
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 *  static CenitNode* - Node representing the literal expression
 *
 * Grammar:
 *  literal_expression = integer_literal ;
 */
static CenitNode* parse_literal_expression(CenitParser *parser)
{
    // By now we just parse integers, particularly uint8
    if (cenit_parser_peek(parser).type != CENIT_TOKEN_INTEGER)
    {
        parser_error(parser, CENIT_ERROR_SYNTAX, "Expecting a literal value", parser->lexer.line, parser->lexer.col);
        return NULL;
    }

    return parse_integer_literal(parser);
}

static CenitNode* parse_array_initializer(CenitParser *parser)
{
    CenitToken temp_token;
    if (!cenit_parser_expects(parser, CENIT_TOKEN_LBRACKET, &temp_token))
    {
        parser_error(parser, CENIT_ERROR_SYNTAX, "Expecting LEFT BRACKET ([)", parser->lexer.line, parser->lexer.col);
        return NULL;
    }

    CenitArrayInitNode *node = fl_malloc(sizeof(CenitArrayInitNode));
    node->base.type = CENIT_NODE_ARRAY_INIT;
    node->base.col = temp_token.col;
    node->base.line = temp_token.line;
    node->values = fl_array_new(sizeof(CenitNode*), 0);


    while (true)
    {
        CenitToken token = cenit_parser_peek(parser);

        if (token.type == CENIT_TOKEN_RBRACKET || token.type == CENIT_TOKEN_EOF)
            break;

        CenitNode *value = parse_expression(parser);

        if (value == NULL)
            goto bad_expression_value;

        node->values = fl_array_append(node->values, &value);

        if (cenit_parser_peek(parser).type == CENIT_TOKEN_COMMA)
            cenit_parser_consume(parser);
    }

    if (!cenit_parser_expects(parser, CENIT_TOKEN_RBRACKET, &temp_token))
    {
        parser_error(parser, CENIT_ERROR_SYNTAX, "Expecting RIGHT BRACKET (])", parser->lexer.line, parser->lexer.col);
        goto missing_bracket;
    }

    return (CenitNode*)node;

    bad_expression_value:
        if (node->values)
            fl_array_free_each(node->values, cenit_node_pointer_free);

    missing_bracket:
        cenit_node_free((CenitNode*)node);
    return NULL;
}

/*
 * Function: parse_expression
 *  Parses a single expression
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 *  CenitNode* - Parsed expression node
 * 
 * Grammar:
 *  expression = array_initializer | literal_expression ;
 *
 */
static CenitNode* parse_expression(CenitParser *parser)
{
    if (cenit_parser_peek(parser).type == CENIT_TOKEN_LBRACKET)
        return parse_array_initializer(parser);

    return parse_literal_expression(parser);
}

/*
 * Function: parse_variable_declaration
 *  Parses a variable declartion
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 *  CenitNode* - Variable declaration node
 * 
 * Grammar:
 *  variable_declaration = 'var' ID ( ':' ID )? '=' expression ';' ;
 *
 */
static CenitNode* parse_variable_declaration(CenitParser *parser)
{
    CenitToken temp_token;

    if (!cenit_parser_expects(parser, CENIT_TOKEN_VAR, &temp_token))
    {
        parser_error(parser, CENIT_ERROR_SYNTAX, "Expecting token 'var'", parser->lexer.line, parser->lexer.col);
        return NULL;
    }
    
    CenitVariableNode *var_node = fl_malloc(sizeof(CenitVariableNode));
    var_node->base.type = CENIT_NODE_VARIABLE;
    var_node->base.col = temp_token.col;
    var_node->base.line = temp_token.line;

    if (!cenit_parser_expects(parser, CENIT_TOKEN_ID, &temp_token))
    {
        parser_error(parser, CENIT_ERROR_SYNTAX, "Variable name cannot be omitted", parser->lexer.line, parser->lexer.col);
        goto missing_name;
    }
    
    var_node->name = fl_cstring_dup_n((const char*)temp_token.value.sequence, temp_token.value.length);

    if (cenit_parser_peek(parser).type == CENIT_TOKEN_COLON)
    {
        cenit_parser_consume(parser);

        // Check if it is an array
        if (cenit_parser_peek(parser).type == CENIT_TOKEN_LBRACKET)
        {
            cenit_parser_consume(parser);

            CenitLiteralNode *literal = (CenitLiteralNode*)parse_integer_literal(parser);
            
            if (literal == NULL)
                goto missing_type;

            var_node->typeinfo.is_array = true;
            switch (literal->typeinfo.type)
            {
                case CENIT_TYPE_UINT8:
                    var_node->typeinfo.elements = (size_t)literal->value.uint8;
                    break;
                default:
                    goto missing_type;
            }

            cenit_node_free((CenitNode*)literal);

            if (!cenit_parser_expects(parser, CENIT_TOKEN_RBRACKET, NULL))
                goto missing_type;
        }
        else
        {
            var_node->typeinfo.is_array = false;
            var_node->typeinfo.elements = 1;
        }
        
        if (!cenit_parser_expects(parser, CENIT_TOKEN_ID, &temp_token))
        {
            parser_error(parser, CENIT_ERROR_SYNTAX, "Missing type name", parser->lexer.line, parser->lexer.col);
            goto missing_type;
        }

        var_node->typeinfo.type = cenit_type_parse_slice(&temp_token.value);
        if (var_node->typeinfo.type == CENIT_TYPE_CUSTOM)
            var_node->typeinfo.name = fl_cstring_dup_n((const char*)temp_token.value.sequence, temp_token.value.length);
    }

    if (!cenit_parser_expects(parser, CENIT_TOKEN_ASSIGNMENT, &temp_token))
    {
        parser_error(parser, CENIT_ERROR_SYNTAX, "Expecting assignment operator ('=')", parser->lexer.line, parser->lexer.col);
        goto missing_assignment;
    }

    var_node->value = parse_expression(parser);

    if (var_node->value == NULL)
        goto missing_assignment;

    if (!cenit_parser_expects(parser, CENIT_TOKEN_SEMICOLON, NULL))
    {
        parser_error(parser, CENIT_ERROR_SYNTAX, "Missing semicolon (';')", parser->lexer.line, parser->lexer.col);
        goto missing_semicolon;
    }

    // Success
    return (CenitNode*)var_node;


    // Cleanup code
    missing_semicolon:
        cenit_node_free(var_node->value);

    missing_assignment:
        if (var_node->typeinfo.name)
            fl_cstring_free(var_node->typeinfo.name);

    missing_type:
        fl_cstring_free(var_node->name);

    missing_name:
        fl_free(var_node);

    return NULL;
}

/*
 * Function: parse_expression_statement
 *  An expression statement is a a expression followed by a semicolon
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 *  CenitNode* - Parsed expression
 *
 * Grammar:
 *  expression_statement = expression ';' ;
 */
static CenitNode* parse_expression_statement(CenitParser *parser)
{
    CenitNode *node = parse_expression(parser);

    if (cenit_parser_expects(parser, CENIT_TOKEN_SEMICOLON, NULL))
        return node;

    parser_error(parser, CENIT_ERROR_SYNTAX, "Missing semicolon (';')", parser->lexer.line, parser->lexer.col);

    // Missing SEMICOLON
    cenit_node_free(node);
    return NULL;
}

/*
 * Function: parse_statement
 *  This function parses multiple type of statements: conditionals,
 *  loops, blocks, expression statements, etc.
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 *  CenitNode* - The parsed statement
 * 
 * Grammar:
 *  statement = expression_statement ;
 */
static CenitNode* parse_statement(CenitParser *parser)
{
    // By now it parses just expression statements, but here we should
    // place conditionals and loops (including their constructions)
    // and blocks
    return parse_expression_statement(parser);
}

/*
 * Function: parse_declaration
 *  This is the top-level non-terminal of the language
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 * CenitNode* - Parsed declaration node
 * 
 * Grammar:
 *  declaration = variable_declaration | statement ;
 *
 */
static CenitNode* parse_declaration(CenitParser *parser)
{
    CenitToken token = cenit_parser_peek(parser);

    // Check for variables, functions, etc
    if (token.type == CENIT_TOKEN_VAR)
        return parse_variable_declaration(parser);

    // If there are no variables or functions declarations,
    // it is a statement
    return parse_statement(parser);
}

/* Public API */
CenitParser cenit_parser_new(const char *source)
{
    return (CenitParser){
        .lexer = cenit_lexer_new(source),
        .errors = NULL
    };
}

void cenit_parser_free(CenitParser *parser)
{
    if (!parser)
        return;

    if (parser->errors)
        fl_array_free_each(parser->errors, error_free);
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


/*
 * Function: cenit_parser_parse
 *  Parse a whole program
 *
 * Parameters:
 *  parser - Parser object
 *
 * Returns:
 *  CenitAst* - The parsed program represented by an AST object
 * 
 * Grammar:
 *  program = declaration *
 *
 */
CenitAst* cenit_parser_parse(CenitParser *parser)
{
    // We use a vector with an initial capacity, if there are more
    // nodes the vector will resize automatically
    FlVector tempvec = fl_vector_new_args((struct FlVectorArgs) {
        .element_size = sizeof(CenitNode*),
        .capacity = 1000
    });

    // Parse the program
    while (cenit_parser_has_input(parser))
    {
        CenitNode *declaration = parse_declaration(parser);

        // If declaration is NULL, something happened, so let's synch
        if (declaration == NULL)
        {
            do {
                CenitToken token = cenit_parser_consume(parser);

                // SEMICOLON is our synchronizing token. On EOF we break
                if (token.type == CENIT_TOKEN_SEMICOLON || token.type == CENIT_TOKEN_EOF)
                    break;

            } while (true);

            // Let's try again
            continue;
        }

        // The "top-level" node is a declaration
        fl_vector_add(tempvec, declaration);
    }

    // Create the ast object
    CenitAst *ast = fl_malloc(sizeof(CenitAst));
    ast->decls = fl_vector_to_array(tempvec);
    fl_vector_free(tempvec);

    return ast;
}

