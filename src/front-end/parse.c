#include <fllib.h>
#include <errno.h>
#include "lexer.h"
#include "parse.h"
#include "parser.h"
#include "ast.h"

#define token_to_string(tokenptr) fl_cstring_dup_n((const char*)(tokenptr)->value.sequence, (tokenptr)->value.length)

#define token_to_typeinfo(tokenptr, typeinfoptr)                            \
    do {                                                                    \
        (typeinfoptr)->type = cenit_type_slice_parse(&(tokenptr)->value);   \
        if ((typeinfoptr)->type == CENIT_TYPE_CUSTOM)                       \
            (typeinfoptr)->name = token_to_string((tokenptr));              \
    } while (0)

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
        cenit_context_error(ctx, ctx->srcinfo->location, CENIT_ERROR_SYNTAX, "Expecting an integer literal");
        return NULL;
    }

    // Create the literal node with the basic information
    CenitLiteralNode *literal_node = fl_malloc(sizeof(CenitLiteralNode));
    literal_node->base.type = CENIT_NODE_LITERAL;
    literal_node->base.location = temp_token.location;

    // This is the integer parsing logic (unsigned integers in base 10 by now)
    char *endptr;
    unsigned long long temp_int = strtoull((const char*)temp_token.value.sequence, &endptr, 10);
    
    // The token length must be equals to the parsed number
    if ((void*)(temp_token.value.sequence + temp_token.value.length) != (void*)endptr)
        goto range_error;

    // Check for specific strtoull errors
    if ((temp_int == ULLONG_MAX && errno == ERANGE) || (temp_int == 0ULL && errno == EINVAL))
    {
        cenit_context_error(ctx, ctx->srcinfo->location, CENIT_ERROR_LARGE_INTEGER, "Integral value is too large");
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
        cenit_context_error(ctx, ctx->srcinfo->location, CENIT_ERROR_LARGE_INTEGER, "Integral value is too large");
        goto range_error;
    }

    // Success
    return (CenitNode*)literal_node;

    // Cleanup code for error conditions
    range_error:    cenit_node_free((CenitNode*)literal_node);

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
 *  CenitNode* - Node representing the literal expression
 *
 * Grammar:
 *  literal_expression = integer_literal ;
 */
static CenitNode* parse_literal_expression(CenitParser *parser, CenitContext *ctx)
{
    // By now we just parse integers, particularly uint8
    if (cenit_parser_peek(parser).type != CENIT_TOKEN_INTEGER)
    {
        cenit_context_error(ctx, ctx->srcinfo->location, CENIT_ERROR_SYNTAX, "Expecting a literal value");
        return NULL;
    }

    return parse_integer_literal(parser, ctx);
}

/*
 * Function: parse_array_initializer
 *  Parses an array initializer
 *
 * Parameters:
 *  parser - Parser object
*  ctx - <CenitContext> object
 *
 * Returns:
 *  CenitNode* - The <CenitArrayInitNode> object
 *
 * Grammar:
 *  array_initializer = '[' ( expression ( ',' expression )* ','? )? ']' ;
 */
static CenitNode* parse_array_initializer(CenitParser *parser, CenitContext *ctx)
{
    CenitToken temp_token;

    // Consume the required left bracket
    if (!cenit_parser_expects(parser, CENIT_TOKEN_LBRACKET, &temp_token))
    {
        cenit_context_error(ctx, ctx->srcinfo->location, CENIT_ERROR_SYNTAX, "Expecting LEFT BRACKET ([)");
        return NULL;
    }

    // Allocate memory for the array node and fill the basic information
    CenitArrayInitNode *node = fl_malloc(sizeof(CenitArrayInitNode));
    node->base.type = CENIT_NODE_ARRAY_INIT;
    node->base.location = temp_token.location;
    node->values = fl_array_new(sizeof(CenitNode*), 0);

    // Array type information: The type is NONE by default and is
    // updated accordingly in the inference pass based on its content
    node->typeinfo.type = CENIT_TYPE_NONE;
    node->typeinfo.name = NULL;
    node->typeinfo.elements = 0;
    node->typeinfo.is_array = true;

    // Keep iterating until get an EOF token or breaking out from inside 
    while (cenit_parser_has_input(parser))
    {
        CenitToken token = cenit_parser_peek(parser);

        // If we find the right bracket, we finished parsing the elements
        if (token.type == CENIT_TOKEN_RBRACKET)
            break;

        // Each element in the array initializer is an expression
        CenitNode *value = parse_expression(parser, ctx);

        // Something happened parsing the element, we need to leave
        if (value == NULL)
            goto bad_expression_value;

        node->values = fl_array_append(node->values, &value);

        // Consume the trailing comma if present
        cenit_parser_consume_if(parser, CENIT_TOKEN_COMMA);
    }

    // If the following token is not the right bracket, we reached the EOF token
    if (!cenit_parser_expects(parser, CENIT_TOKEN_RBRACKET, &temp_token))
    {
        cenit_context_error(ctx, ctx->srcinfo->location, CENIT_ERROR_SYNTAX, "Expecting RIGHT BRACKET (])");
        goto missing_bracket;
    }

    // Update the number of elements
    node->typeinfo.elements = fl_array_length(node->values);

    // Return the parsed array initializer
    return (CenitNode*)node;

    // Cleanup code for error conditions
    bad_expression_value:   cenit_node_array_free(node->values);
    missing_bracket:        cenit_node_free((CenitNode*)node);

    return NULL;
}

/*
 * Function: parse_expression
 *  Parses a single expression
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <CenitContext> object
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
 *  ctx - <CenitContext> object
 *
 * Returns:
 *  CenitNode* - Variable declaration node
 * 
 * Grammar:
 *  variable_declaration = 'var' ID ( ':' ( '[' integer_literal ']' )? ID )? '=' expression ';' ;
 *
 */
static CenitNode* parse_variable_declaration(CenitParser *parser, CenitContext *ctx)
{
    CenitToken temp_token;

    // The 'var' token is required
    if (!cenit_parser_expects(parser, CENIT_TOKEN_VAR, &temp_token))
    {
        cenit_context_error(ctx, ctx->srcinfo->location, CENIT_ERROR_SYNTAX, "Expecting token 'var'");
        return NULL;
    }
    
    // Allocate the memory and the base information
    CenitVariableNode *var_node = fl_malloc(sizeof(CenitVariableNode));
    var_node->base.type = CENIT_NODE_VARIABLE;
    var_node->base.location = temp_token.location;

    // The variable name is required to be present and be valid
    if (!cenit_parser_expects(parser, CENIT_TOKEN_ID, &temp_token))
    {
        if (cenit_parser_peek(parser).type == CENIT_TOKEN_UNKNOWN)
        {
            struct FlSlice slice = cenit_parser_peek(parser).value;
            size_t length = slice.length;
            const char *str = (const char*)slice.sequence;
            cenit_context_error(ctx, ctx->srcinfo->location, CENIT_ERROR_SYNTAX, "'%.*s' is not a valid identifier", length, str);
        }
        else
        {
            cenit_context_error(ctx, ctx->srcinfo->location, CENIT_ERROR_SYNTAX, "Missing variable name");
        }
        goto missing_name;
    }
    
    // Get the variable name from the token
    var_node->name = token_to_string(&temp_token);

    // If the COLON token is present, we need to parse the type information
    if (cenit_parser_consume_if(parser, CENIT_TOKEN_COLON))
    {
        // Check if it is an array
        if (cenit_parser_consume_if(parser, CENIT_TOKEN_LBRACKET))
        {
            var_node->typeinfo.is_array = true;

            // Parse the array size that must be an integer literal
            CenitLiteralNode *literal = (CenitLiteralNode*)parse_integer_literal(parser, ctx);
            
            if (literal == NULL)
                goto missing_type;

            // If the literal node is valid, we get the size form its value
            switch (literal->typeinfo.type)
            {
                case CENIT_TYPE_UINT8:
                    var_node->typeinfo.elements = (size_t)literal->value.uint8;
                    break;
                default:
                    goto missing_type;
            }

            // We can safely free the size here
            cenit_node_free((CenitNode*)literal);

            // The closing bracket is required
            if (!cenit_parser_expects(parser, CENIT_TOKEN_RBRACKET, NULL))
                goto missing_type;
        }
        else
        {
            // The type declaration is a single element
            var_node->typeinfo.is_array = false;
            var_node->typeinfo.elements = 1;
        }
        
        // The type name is required, so let's get it
        if (!cenit_parser_expects(parser, CENIT_TOKEN_ID, &temp_token))
        {
            cenit_context_error(ctx, ctx->srcinfo->location, CENIT_ERROR_SYNTAX, "Missing type name");
            goto missing_type;
        }

        // Fill the node's <CenitTypeInfo> object
        token_to_typeinfo(&temp_token, &var_node->typeinfo);
    }

    // We don't allow variables without initializers, the ASSIGN token is required
    if (!cenit_parser_expects(parser, CENIT_TOKEN_ASSIGN, &temp_token))
    {
        cenit_context_error(ctx, ctx->srcinfo->location, CENIT_ERROR_SYNTAX, "Expecting assignment operator ('=')");
        goto missing_assignment;
    }

    // The right-hand side value is an expression
    var_node->value = parse_expression(parser, ctx);

    // As noted before, the initialization expression is required
    if (var_node->value == NULL)
        goto missing_assignment;

    // The variable declaration ends with a semicolon
    if (!cenit_parser_expects(parser, CENIT_TOKEN_SEMICOLON, NULL))
    {
        cenit_context_error(ctx, ctx->srcinfo->location, CENIT_ERROR_SYNTAX, "Missing semicolon (';')");
        goto missing_semicolon;
    }

    // Success
    return (CenitNode*)var_node;

    // Cleanup code for error conditions
    missing_semicolon:   cenit_node_free(var_node->value);
    missing_assignment:  fl_cstring_free(var_node->typeinfo.name);
    missing_type:        fl_cstring_free(var_node->name);
    missing_name:        fl_free(var_node);

    return NULL;
}

/*
 * Function: parse_expression_statement
 *  An expression statement is a a expression followed by a semicolon
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <CenitContext> object
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

    if (!cenit_parser_expects(parser, CENIT_TOKEN_SEMICOLON, NULL))
    {
        cenit_context_error(ctx, ctx->srcinfo->location, CENIT_ERROR_SYNTAX, "Missing semicolon (';')");
        goto missing_semicolon;
    }

    // Success
    return node;

    // Cleanup code for error conditions
    missing_semicolon:  cenit_node_free(node);

    return NULL;
}

/*
 * Function: parse_statement
 *  This function parses multiple type of statements: conditionals,
 *  loops, blocks, expression statements, etc.
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <CenitContext> object
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
 *  ctx - <CenitContext> object
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
 * Function: cenit_parse_source
 *  While the parser has input, we call the <parse_declaration> function
 *  that is the top-level function that knows how to "traverse" the code
 */
bool cenit_parse_source(CenitContext *ctx)
{
    // We use a vector with an initial capacity, if there are more
    // nodes the vector will resize automatically
    FlVector tempvec = fl_vector_new_args((struct FlVectorArgs) {
        .element_size = sizeof(CenitNode*),
        .capacity = 1000
    });

    CenitParser parser = cenit_parser_new(ctx->srcinfo);

    // Each iteration processes a declaration which is a subtree of the
    // final ast object
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

    // Create the CenitAst object
    ctx->ast = fl_malloc(sizeof(CenitAst));
    ctx->ast->decls = fl_vector_to_array(tempvec);
    fl_vector_free(tempvec);

    return !ctx->errors;
}
