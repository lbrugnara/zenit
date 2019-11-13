#include <fllib.h>
#include <errno.h>
#include "lexer.h"
#include "parse.h"
#include "parser.h"
#include "ast.h"

#define token_to_string(tokenptr) fl_cstring_dup_n((const char*)(tokenptr)->value.sequence, (tokenptr)->value.length)

#define fill_typeinfo_from_token(typeinfoptr, tokenptr)                     \
    do {                                                                    \
        (typeinfoptr)->type = zenit_type_slice_parse(&(tokenptr)->value);   \
        if ((typeinfoptr)->type == ZENIT_TYPE_CUSTOM)                       \
            (typeinfoptr)->name = token_to_string((tokenptr));              \
    } while (0)

/* Private API */
static struct ZenitNode* parse_integer_literal(struct ZenitParser *parser, struct ZenitContext *ctx);
static struct ZenitNode* parse_literal_expression(struct ZenitParser *parser, struct ZenitContext *ctx);
static struct ZenitNode* parse_array_initializer(struct ZenitParser *parser, struct ZenitContext *ctx);
static struct ZenitNode* parse_expression(struct ZenitParser *parser, struct ZenitContext *ctx);
static struct ZenitNode* parse_variable_declaration(struct ZenitParser *parser, struct ZenitContext *ctx);
static struct ZenitNode* parse_expression_statement(struct ZenitParser *parser, struct ZenitContext *ctx);
static struct ZenitNode* parse_statement(struct ZenitParser *parser, struct ZenitContext *ctx);
static struct ZenitNode* parse_declaration(struct ZenitParser *parser, struct ZenitContext *ctx);


/*
 * Function: parse_typeinfo
 *  Helper function that parses the type information and populates the provided <struct ZenitTypeInfo> object
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *  typeinfo - <struct ZenitTypeInfo> object to be populated with the type information
 *
 * Returns:
 *  bool - If the type is present and is valid this function returns *true*. Otherwise returns *false*.
 *
 */
static bool parse_typeinfo(struct ZenitParser *parser, struct ZenitContext *ctx, struct ZenitTypeInfo *typeinfo)
{
    // Check if it is an array
    if (zenit_parser_consume_if(parser, ZENIT_TOKEN_LBRACKET))
    {
        typeinfo->is_array = true;

        // Parse the array size that must be an integer literal
        struct ZenitLiteralNode *literal = (struct ZenitLiteralNode*)parse_integer_literal(parser, ctx);
        
        if (literal == NULL)
            return false;

        // If the literal node is valid, we get the size form its value
        switch (literal->base.typeinfo.type)
        {
            case ZENIT_TYPE_UINT8:
                typeinfo->elements = (size_t)literal->value.uint8;
                break;

            case ZENIT_TYPE_UINT16:
                typeinfo->elements = (size_t)literal->value.uint16;
                break;
            
            default:
                return false;
        }

        // We can safely free the size here
        zenit_node_free((struct ZenitNode*)literal);

        // The closing bracket is required
        if (!zenit_parser_expects(parser, ZENIT_TOKEN_RBRACKET, NULL))
            return false;
    }
    else
    {
        // The type declaration is a single element
        typeinfo->is_array = false;
        typeinfo->elements = 1;
    }
    
    // Check if it is a reference
    typeinfo->is_ref = false;
    if (zenit_parser_consume_if(parser, ZENIT_TOKEN_AMPERSAND))
        typeinfo->is_ref = true;

    // The type name is required, so let's get it
    struct ZenitToken temp_token;
    if (!zenit_parser_expects(parser, ZENIT_TOKEN_ID, &temp_token))
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Missing type name");
        return false;
    }

    // Fill the node's <struct ZenitTypeInfo> object
    fill_typeinfo_from_token(typeinfo, &temp_token);

    return true;
}

/*
 * Function: parse_integer_literal
 *  Parses an integer literal
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 *  struct ZenitNode* - The parsed integer literal
 *
 * Grammar:
 *  integer_literal = ( '+' | '-' )? INTEGER
 */
static struct ZenitNode* parse_integer_literal(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    struct ZenitToken temp_token;

    // By now we just parse integers, particularly uint8
    if (!zenit_parser_expects(parser, ZENIT_TOKEN_INTEGER, &temp_token))
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Expecting an integer literal");
        return NULL;
    }

    // Create the literal node with the basic information
    struct ZenitLiteralNode *literal_node = fl_malloc(sizeof(struct ZenitLiteralNode));
    literal_node->base.type = ZENIT_NODE_LITERAL;
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
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_LARGE_INTEGER, "Integral value is too large");
        goto range_error;
    }

    // Check the type of the literal value
    if (temp_int <= UINT8_MAX)
    {
        literal_node->value.uint8 = (uint8_t)temp_int;
        literal_node->base.typeinfo.type = ZENIT_TYPE_UINT8;
        literal_node->base.typeinfo.is_array = false;
        literal_node->base.typeinfo.elements = 1;
    }
    else if (temp_int <= UINT16_MAX)
    {
        literal_node->value.uint16 = (uint16_t)temp_int;
        literal_node->base.typeinfo.type = ZENIT_TYPE_UINT16;
        literal_node->base.typeinfo.is_array = false;
        literal_node->base.typeinfo.elements = 1;
    }
    else
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_LARGE_INTEGER, "Integral value is too large");
        goto range_error;
    }

    // Success
    return (struct ZenitNode*)literal_node;

    // Cleanup code for error conditions
    range_error:    zenit_node_free((struct ZenitNode*)literal_node);

    return NULL;
}

/*
 * Function: parse_literal_expression
 *  Parses all the literal expressions supported by the language
 *
 * Parameters:
 *  parser - Parser object
*  ctx - <struct ZenitContext> object
 *
 * Returns:
 *  struct ZenitNode* - Node representing the literal expression
 *
 * Grammar:
 *  literal_expression = integer_literal ;
 */
static struct ZenitNode* parse_literal_expression(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    // By now we just parse integers, particularly uint8
    if (zenit_parser_peek(parser).type != ZENIT_TOKEN_INTEGER)
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Expecting a literal value");
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
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 *  struct ZenitNode* - The <struct ZenitArrayInitNode> object
 *
 * Grammar:
 *  array_initializer = '[' ( expression ( ',' expression )* ','? )? ']' ;
 */
static struct ZenitNode* parse_array_initializer(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    struct ZenitToken temp_token;

    // Consume the required left bracket
    if (!zenit_parser_expects(parser, ZENIT_TOKEN_LBRACKET, &temp_token))
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Expecting LEFT BRACKET ([)");
        return NULL;
    }

    // Allocate memory for the array node and fill the basic information
    struct ZenitArrayInitNode *node = fl_malloc(sizeof(struct ZenitArrayInitNode));
    node->base.type = ZENIT_NODE_ARRAY_INIT;
    node->base.location = temp_token.location;
    node->elements = fl_array_new(sizeof(struct ZenitNode*), 0);

    // Array type information: The type is NONE by default and is
    // updated accordingly in the inference pass based on its content
    node->base.typeinfo.type = ZENIT_TYPE_NONE;
    node->base.typeinfo.name = NULL;
    node->base.typeinfo.elements = 0;
    node->base.typeinfo.is_array = true;

    // Keep iterating until get an EOF token or breaking out from inside 
    while (zenit_parser_has_input(parser))
    {
        struct ZenitToken token = zenit_parser_peek(parser);

        // If we find the right bracket, we finished parsing the elements
        if (token.type == ZENIT_TOKEN_RBRACKET)
            break;

        // Each element in the array initializer is an expression
        struct ZenitNode *value = parse_expression(parser, ctx);

        // Something happened parsing the element, we need to leave
        if (value == NULL)
            goto bad_expression_value;

        node->elements = fl_array_append(node->elements, &value);

        // Consume the trailing comma if present
        zenit_parser_consume_if(parser, ZENIT_TOKEN_COMMA);
    }

    // If the following token is not the right bracket, we reached the EOF token
    if (!zenit_parser_expects(parser, ZENIT_TOKEN_RBRACKET, &temp_token))
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Expecting RIGHT BRACKET (])");
        goto missing_bracket;
    }

    // Update the number of elements
    node->base.typeinfo.elements = fl_array_length(node->elements);

    // Return the parsed array initializer
    return (struct ZenitNode*)node;

    // Cleanup code for error conditions
    bad_expression_value:   zenit_node_array_free(node->elements);
    missing_bracket:        zenit_node_free((struct ZenitNode*)node);

    return NULL;
}

/*
 * Function: parse_unary_expression
 *  Parses a unary expression
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 *  struct ZenitNode* - Parsed unary expression
 * 
 * Grammar:
 *  unary_expression = '&' expression | ID | literal_expression ;
 *
 */
static struct ZenitNode* parse_unary_expression(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    struct ZenitToken temp_token = zenit_parser_peek(parser);
    if (temp_token.type == ZENIT_TOKEN_AMPERSAND)
    {
        zenit_parser_consume(parser);
        struct ZenitUnaryRefNode *ref_node = fl_malloc(sizeof(struct ZenitUnaryRefNode));
        ref_node->base.type = ZENIT_NODE_UNARY_REF;
        ref_node->base.location = temp_token.location;
        
        ref_node->expression = parse_expression(parser, ctx);

        if (!ref_node->expression)
        {
            zenit_node_free((struct ZenitNode*)ref_node);
            return NULL;
        }

        zenit_type_copy(&ref_node->base.typeinfo, &ref_node->expression->typeinfo);
        ref_node->base.typeinfo.is_ref = true;

        return (struct ZenitNode*)ref_node;
    }
    else if (temp_token.type == ZENIT_TOKEN_ID)
    {
        zenit_parser_consume(parser);
        struct ZenitIdentifierNode *id_node = fl_malloc(sizeof(struct ZenitIdentifierNode));
        id_node->base.type = ZENIT_NODE_IDENTIFIER;
        id_node->base.location = temp_token.location;
        id_node->name = token_to_string(&temp_token);

        if (!id_node->name)
        {
            zenit_node_free((struct ZenitNode*)id_node);
            return NULL;
        }

        return (struct ZenitNode*)id_node;
    }

    return parse_literal_expression(parser, ctx);
}

/*
 * Function: parse_expression
 *  Parses a single expression
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 *  struct ZenitNode* - Parsed expression node
 * 
 * Grammar:
 *  expression = array_initializer | unary_expression ;
 *
 */
static struct ZenitNode* parse_expression(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    if (zenit_parser_peek(parser).type == ZENIT_TOKEN_LBRACKET)
        return parse_array_initializer(parser, ctx);

    return parse_unary_expression(parser, ctx);
}

/*
 * Function: parse_expression_statement
 *  An expression statement is a a expression followed by a semicolon
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 *  struct ZenitNode* - Parsed expression
 *
 * Grammar:
 *  expression_statement = expression ';' ;
 */
static struct ZenitNode* parse_expression_statement(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    struct ZenitNode *node = parse_expression(parser, ctx);

    if (!zenit_parser_expects(parser, ZENIT_TOKEN_SEMICOLON, NULL))
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Missing semicolon (';')");
        goto missing_semicolon;
    }

    // Success
    return node;

    // Cleanup code for error conditions
    missing_semicolon:  zenit_node_free(node);

    return NULL;
}

/*
 * Function: parse_statement
 *  This function parses multiple type of statements: conditionals,
 *  loops, blocks, expression statements, etc.
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 *  struct ZenitNode* - The parsed statement
 * 
 * Grammar:
 *  statement = expression_statement ;
 */
static struct ZenitNode* parse_statement(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    // By now it parses just expression statements, but here we should
    // place conditionals and loops (including their constructions)
    // and blocks
    return parse_expression_statement(parser, ctx);
}

/*
 * Function: parse_variable_declaration
 *  Parses a variable declartion
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 *  struct ZenitNode* - Variable declaration node
 * 
 * Grammar:
 *  variable_declaration = 'var' ID ( ':' ( '[' integer_literal ']' )? '&'? ID )? '=' expression ';' ;
 *
 */
static struct ZenitNode* parse_variable_declaration(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    struct ZenitToken temp_token;

    // The 'var' token is required
    if (!zenit_parser_expects(parser, ZENIT_TOKEN_VAR, &temp_token))
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Expecting token 'var'");
        return NULL;
    }
    
    // Allocate the memory and the base information
    struct ZenitVariableNode *var_node = fl_malloc(sizeof(struct ZenitVariableNode));
    var_node->base.type = ZENIT_NODE_VARIABLE;
    var_node->base.location = temp_token.location;

    // The variable name is required to be present and be valid
    if (!zenit_parser_expects(parser, ZENIT_TOKEN_ID, &temp_token))
    {
        if (zenit_parser_peek(parser).type == ZENIT_TOKEN_UNKNOWN)
        {
            struct FlSlice slice = zenit_parser_peek(parser).value;
            size_t length = slice.length;
            const char *str = (const char*)slice.sequence;
            zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "'%.*s' is not a valid identifier", length, str);
        }
        else
        {
            zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Missing variable name");
        }
        goto missing_name;
    }
    
    // Get the variable name from the token
    var_node->name = token_to_string(&temp_token);

    // If the COLON token is present, we need to parse the type information
    if (zenit_parser_consume_if(parser, ZENIT_TOKEN_COLON))
    {
        if (!parse_typeinfo(parser, ctx, &var_node->base.typeinfo))
            goto missing_type;
    }

    // We don't allow variables without initializers, the ASSIGN token is required
    if (!zenit_parser_expects(parser, ZENIT_TOKEN_ASSIGN, &temp_token))
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Expecting assignment operator ('=')");
        goto missing_assignment;
    }

    // The right-hand side value is an expression
    var_node->value = parse_expression(parser, ctx);

    // As noted before, the initialization expression is required
    if (var_node->value == NULL)
        goto missing_assignment;

    // The variable declaration ends with a semicolon
    if (!zenit_parser_expects(parser, ZENIT_TOKEN_SEMICOLON, NULL))
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Missing semicolon (';')");
        goto missing_semicolon;
    }

    // Success
    return (struct ZenitNode*)var_node;

    // Cleanup code for error conditions
    missing_semicolon:   zenit_node_free(var_node->value);
    missing_assignment:  fl_cstring_free(var_node->base.typeinfo.name);
    missing_type:        fl_cstring_free(var_node->name);
    missing_name:        fl_free(var_node);

    return NULL;
}

/*
 * Function: parse_attribute_declaration
 *  This is the top-level function that parses a sequence of attributes declarations
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 * struct ZenitNode* - Parsed attributes declaration node
 * 
 * Grammar:
 *  declaration = parse_attribute_declaration | variable_declaration | statement ;
 *
 */
static struct ZenitNode* parse_attribute_declaration(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    struct ZenitAttributeNode *attribute = fl_malloc(sizeof(struct ZenitAttributeNode));

    struct ZenitToken hash_token;
    struct ZenitToken name_token;

    if (!zenit_parser_expects(parser, ZENIT_TOKEN_HASH, &hash_token))   goto parsing_error;
    if (!zenit_parser_expects(parser, ZENIT_TOKEN_LBRACKET, NULL))      goto parsing_error;
    if (!zenit_parser_expects(parser, ZENIT_TOKEN_ID, &name_token))     goto parsing_error;
    
    attribute->base.type = ZENIT_NODE_ATTRIBUTE;
    attribute->base.location = hash_token.location;
    attribute->name = token_to_string(&name_token);

    if (zenit_parser_peek(parser).type == ZENIT_TOKEN_LPAREN)
    {
        zenit_parser_expects(parser, ZENIT_TOKEN_LPAREN, NULL);

        while (zenit_parser_peek(parser).type == ZENIT_TOKEN_ID)
        {
            struct ZenitToken prop_name = zenit_parser_consume(parser);
            if (!zenit_parser_expects(parser, ZENIT_TOKEN_COLON, NULL))   goto parsing_error;

            // FIXME: We could restrict this to primitive types, by now it's ok
            struct ZenitNode *value = parse_expression(parser, ctx);

            if (value == NULL)
                goto parsing_error;

            struct ZenitAttributePropertyNode property = {
                .name = token_to_string(&prop_name),
                .value = value
            };

            if (!attribute->properties)
                attribute->properties = fl_array_new(sizeof(struct ZenitAttributePropertyNode), 0);
            
            attribute->properties = fl_array_append(attribute->properties, &property);

            if (zenit_parser_peek(parser).type != ZENIT_TOKEN_RPAREN && !zenit_parser_expects(parser, ZENIT_TOKEN_COMMA, NULL))
                goto parsing_error;
        }

        zenit_parser_expects(parser, ZENIT_TOKEN_RPAREN, NULL);
    }

    if (!zenit_parser_expects(parser, ZENIT_TOKEN_RBRACKET, NULL))      goto parsing_error;

    // Success
    return (struct ZenitNode*)attribute;

    // FIXME: Implement the free function for attributes list
    parsing_error: zenit_node_free((struct ZenitNode*)attribute);

    return NULL;
}

/*
 * Function: parse_declaration
 *  This is the top-level non-terminal of the language
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 * struct ZenitNode* - Parsed declaration node
 * 
 * Grammar:
 *  declaration = parse_attribute_declaration | variable_declaration | statement ;
 *
 */
static struct ZenitNode* parse_declaration(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    struct ZenitAttributeNode **attributes = NULL;

    while (zenit_parser_peek(parser).type == ZENIT_TOKEN_HASH)
    {
        if (attributes == NULL)
            attributes = fl_array_new(sizeof(struct ZenitAttributeNode*), 0);

        struct ZenitAttributeNode *attribute = (struct ZenitAttributeNode*)parse_attribute_declaration(parser, ctx);
        attributes = fl_array_append(attributes, &attribute);
    }

    // Check for variables, functions, etc
    if (zenit_parser_peek(parser).type == ZENIT_TOKEN_VAR)
    {
        struct ZenitNode *vardecl = parse_variable_declaration(parser, ctx);

        if (vardecl != NULL)
            ((struct ZenitVariableNode*)vardecl)->attributes = attributes;

        return vardecl;
    }

    if (attributes != NULL)
        zenit_context_error(ctx, attributes[0]->base.location, ZENIT_ERROR_SYNTAX, "Invalid use of attributes");

    // If there are no variables or functions declarations,
    // it is a statement
    return parse_statement(parser, ctx);
}

/*
 * Function: zenit_parse_source
 *  While the parser has input, we call the <parse_declaration> function
 *  that is the top-level function that knows how to "traverse" the code
 */
bool zenit_parse_source(struct ZenitContext *ctx)
{
    // We use a vector with an initial capacity, if there are more
    // nodes the vector will resize automatically
    FlVector tempvec = fl_vector_new_args((struct FlVectorArgs) {
        .element_size = sizeof(struct ZenitNode*),
        .capacity = 1000
    });

    struct ZenitParser parser = zenit_parser_new(ctx->srcinfo);

    // Each iteration processes a declaration which is a subtree of the
    // final ast object
    while (zenit_parser_has_input(&parser))
    {
        struct ZenitNode *declaration = parse_declaration(&parser, ctx);

        // If declaration is NULL, something happened, so let's synch
        if (declaration == NULL)
        {
            do {
                struct ZenitToken token = zenit_parser_consume(&parser);

                // SEMICOLON is our synchronizing token. On EOF we break
                if (token.type == ZENIT_TOKEN_SEMICOLON || token.type == ZENIT_TOKEN_EOF)
                    break;

            } while (true);

            // Let's try again
            continue;
        }

        // The "top-level" node is a declaration
        fl_vector_add(tempvec, declaration);
    }

    // Create the struct ZenitAst object
    ctx->ast = fl_malloc(sizeof(struct ZenitAst));
    ctx->ast->decls = fl_vector_to_array(tempvec);
    fl_vector_free(tempvec);

    return !ctx->errors;
}
