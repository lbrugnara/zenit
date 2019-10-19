#include <fllib.h>
#include <errno.h>
#include "lexer.h"
#include "parse.h"
#include "parser.h"
#include "ast.h"

/* Private API */
static CenitNode* parse_integer_literal(CenitParser *parser, CenitContext *ctx);
static CenitNode* parse_literal_expression(CenitParser *parser, CenitContext *ctx);
static CenitNode* parse_array_initializer(CenitParser *parser, CenitContext *ctx);
static CenitNode* parse_expression(CenitParser *parser, CenitContext *ctx);
static CenitNode* parse_variable_declaration(CenitParser *parser, CenitContext *ctx);
static CenitNode* parse_expression_statement(CenitParser *parser, CenitContext *ctx);
static CenitNode* parse_statement(CenitParser *parser, CenitContext *ctx);
static CenitNode* parse_declaration(CenitParser *parser, CenitContext *ctx);

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
static CenitNode* parse_integer_literal(CenitParser *parser, CenitContext *ctx)
{
    CenitToken temp_token;

    // By now we just parse integers, particularly uint8
    if (!cenit_parser_expects(parser, CENIT_TOKEN_INTEGER, &temp_token))
    {
        cenit_context_error(ctx, CENIT_ERROR_SYNTAX, parser->lexer.line, parser->lexer.col, "Expecting an integer literal");
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
        cenit_context_error(ctx, CENIT_ERROR_LARGE_INTEGER, parser->lexer.line, parser->lexer.col, "Integral value is too large");
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
        cenit_context_error(ctx, CENIT_ERROR_LARGE_INTEGER, parser->lexer.line, parser->lexer.col, "Integral value is too large");
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
static CenitNode* parse_literal_expression(CenitParser *parser, CenitContext *ctx)
{
    // By now we just parse integers, particularly uint8
    if (cenit_parser_peek(parser).type != CENIT_TOKEN_INTEGER)
    {
        cenit_context_error(ctx, CENIT_ERROR_SYNTAX, parser->lexer.line, parser->lexer.col, "Expecting a literal value");
        return NULL;
    }

    return parse_integer_literal(parser, ctx);
}

static CenitNode* parse_array_initializer(CenitParser *parser, CenitContext *ctx)
{
    CenitToken temp_token;
    if (!cenit_parser_expects(parser, CENIT_TOKEN_LBRACKET, &temp_token))
    {
        cenit_context_error(ctx, CENIT_ERROR_SYNTAX, parser->lexer.line, parser->lexer.col, "Expecting LEFT BRACKET ([)");
        return NULL;
    }

    CenitArrayInitNode *node = fl_malloc(sizeof(CenitArrayInitNode));
    node->base.type = CENIT_NODE_ARRAY_INIT;
    node->base.col = temp_token.col;
    node->base.line = temp_token.line;
    node->values = fl_array_new(sizeof(CenitNode*), 0);

    // Array type information: The type is NONE by default and is
    // updated accordingly in the inference pass based on its content
    node->typeinfo.name = NULL;
    node->typeinfo.elements = 0;
    node->typeinfo.is_array = true;
    node->typeinfo.type = CENIT_TYPE_NONE;


    while (true)
    {
        CenitToken token = cenit_parser_peek(parser);

        if (token.type == CENIT_TOKEN_RBRACKET || token.type == CENIT_TOKEN_EOF)
            break;

        CenitNode *value = parse_expression(parser, ctx);

        if (value == NULL)
            goto bad_expression_value;

        node->values = fl_array_append(node->values, &value);

        if (cenit_parser_peek(parser).type == CENIT_TOKEN_COMMA)
            cenit_parser_consume(parser);
    }

    if (!cenit_parser_expects(parser, CENIT_TOKEN_RBRACKET, &temp_token))
    {
        cenit_context_error(ctx, CENIT_ERROR_SYNTAX, parser->lexer.line, parser->lexer.col, "Expecting RIGHT BRACKET (])");
        goto missing_bracket;
    }

    // Update the number of elements
    node->typeinfo.elements = fl_array_length(node->values);

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
static CenitNode* parse_expression(CenitParser *parser, CenitContext *ctx)
{
    if (cenit_parser_peek(parser).type == CENIT_TOKEN_LBRACKET)
        return parse_array_initializer(parser, ctx);

    return parse_literal_expression(parser, ctx);
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
static CenitNode* parse_variable_declaration(CenitParser *parser, CenitContext *ctx)
{
    CenitToken temp_token;

    if (!cenit_parser_expects(parser, CENIT_TOKEN_VAR, &temp_token))
    {
        cenit_context_error(ctx, CENIT_ERROR_SYNTAX, parser->lexer.line, parser->lexer.col, "Expecting token 'var'");
        return NULL;
    }
    
    CenitVariableNode *var_node = fl_malloc(sizeof(CenitVariableNode));
    var_node->base.type = CENIT_NODE_VARIABLE;
    var_node->base.col = temp_token.col;
    var_node->base.line = temp_token.line;

    if (!cenit_parser_expects(parser, CENIT_TOKEN_ID, &temp_token))
    {
        if (cenit_parser_peek(parser).type == CENIT_TOKEN_UNKNOWN)
        {
            struct FlSlice slice = cenit_parser_peek(parser).value;
            size_t length = slice.length;
            const char *str = (const char*)slice.sequence;
            cenit_context_error(ctx, CENIT_ERROR_SYNTAX,  parser->lexer.line, parser->lexer.col, "'%.*s' is not a valid identifier", length, str);
        }
        else
        {
            cenit_context_error(ctx, CENIT_ERROR_SYNTAX, parser->lexer.line, parser->lexer.col, "Missing variable name");
        }
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

            CenitLiteralNode *literal = (CenitLiteralNode*)parse_integer_literal(parser, ctx);
            
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
            cenit_context_error(ctx, CENIT_ERROR_SYNTAX, parser->lexer.line, parser->lexer.col, "Missing type name");
            goto missing_type;
        }

        var_node->typeinfo.type = cenit_type_parse_slice(&temp_token.value);
        if (var_node->typeinfo.type == CENIT_TYPE_CUSTOM)
            var_node->typeinfo.name = fl_cstring_dup_n((const char*)temp_token.value.sequence, temp_token.value.length);
    }

    if (!cenit_parser_expects(parser, CENIT_TOKEN_ASSIGNMENT, &temp_token))
    {
        cenit_context_error(ctx, CENIT_ERROR_SYNTAX, parser->lexer.line, parser->lexer.col, "Expecting assignment operator ('=')");
        goto missing_assignment;
    }

    var_node->value = parse_expression(parser, ctx);

    if (var_node->value == NULL)
        goto missing_assignment;

    if (!cenit_parser_expects(parser, CENIT_TOKEN_SEMICOLON, NULL))
    {
        cenit_context_error(ctx, CENIT_ERROR_SYNTAX, parser->lexer.line, parser->lexer.col, "Missing semicolon (';')");
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
static CenitNode* parse_expression_statement(CenitParser *parser, CenitContext *ctx)
{
    CenitNode *node = parse_expression(parser, ctx);

    if (cenit_parser_expects(parser, CENIT_TOKEN_SEMICOLON, NULL))
        return node;

    cenit_context_error(ctx, CENIT_ERROR_SYNTAX, parser->lexer.line, parser->lexer.col, "Missing semicolon (';')");

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
static CenitNode* parse_statement(CenitParser *parser, CenitContext *ctx)
{
    // By now it parses just expression statements, but here we should
    // place conditionals and loops (including their constructions)
    // and blocks
    return parse_expression_statement(parser, ctx);
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
static CenitNode* parse_declaration(CenitParser *parser, CenitContext *ctx)
{
    CenitToken token = cenit_parser_peek(parser);

    // Check for variables, functions, etc
    if (token.type == CENIT_TOKEN_VAR)
        return parse_variable_declaration(parser, ctx);

    // If there are no variables or functions declarations,
    // it is a statement
    return parse_statement(parser, ctx);
}

/*
 * Function: cenit_parse_string
 *  Parse a whole program
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - Context object
 *
 * Returns:
 *  CenitAst* - The parsed program represented by an AST object
 * 
 * Grammar:
 *  program = declaration *
 *
 */
bool cenit_parse_string(CenitContext *ctx, const char *source)
{
    // We use a vector with an initial capacity, if there are more
    // nodes the vector will resize automatically
    FlVector tempvec = fl_vector_new_args((struct FlVectorArgs) {
        .element_size = sizeof(CenitNode*),
        .capacity = 1000
    });

    CenitParser parser = cenit_parser_new(source);

    // Parse the program
    while (cenit_parser_has_input(&parser))
    {
        CenitNode *declaration = parse_declaration(&parser, ctx);

        // If declaration is NULL, something happened, so let's synch
        if (declaration == NULL)
        {
            do {
                CenitToken token = cenit_parser_consume(&parser);

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
    ctx->ast = fl_malloc(sizeof(CenitAst));
    ctx->ast->decls = fl_vector_to_array(tempvec);
    fl_vector_free(tempvec);

    return !ctx->errors;
}

