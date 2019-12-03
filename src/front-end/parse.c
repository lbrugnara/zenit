#include <fllib.h>
#include <errno.h>
#include <limits.h>
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
 * Grammar:
 *  type_information = ( '[' integer_literal ']' )? '&'? ID
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
    struct ZenitLiteralNode *literal_node = NULL;

    // This is the integer parsing logic (unsigned integers in base 10 by now)
    char *endptr;
    unsigned long long temp_int = strtoull((const char*)temp_token.value.sequence, &endptr, 10);
    
    // The token length must be equals to the parsed number
    if ((void*)(temp_token.value.sequence + temp_token.value.length) != (void*)endptr)
        return NULL;

    // Check for specific strtoull errors
    if ((temp_int == ULLONG_MAX && errno == ERANGE) || (temp_int == 0ULL && errno == EINVAL))
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_LARGE_INTEGER, "Integral value is too large");
        return NULL;
    }

    // Check the type of the literal value
    if (temp_int <= UINT8_MAX)
    {
        literal_node = zenit_node_literal_new(temp_token.location, ZENIT_TYPE_UINT8, (union ZenitLiteralValue){ .uint8 = (uint8_t)temp_int });
    }
    else if (temp_int <= UINT16_MAX)
    {
        literal_node = zenit_node_literal_new(temp_token.location, ZENIT_TYPE_UINT16, (union ZenitLiteralValue){ .uint16 = (uint16_t)temp_int });
    }
    else
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_LARGE_INTEGER, "Integral value is too large");
        return NULL;
    }

    // Success
    return (struct ZenitNode*)literal_node;
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
 *  struct ZenitNode* - The <struct ZenitArrayNode> object
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
    struct ZenitArrayNode *node = zenit_node_array_new(temp_token.location);

    if (node == NULL)
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_INTERNAL, "Could not initialize an array initializer node");
        return NULL;
    }

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
    bad_expression_value:
    missing_bracket:
        zenit_node_array_free(node);

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

    // Check for a reference expression
    if (temp_token.type == ZENIT_TOKEN_AMPERSAND)
    {
        zenit_parser_consume(parser);

        struct ZenitUnaryRefNode *ref_node = zenit_node_unary_ref_new(temp_token.location, parse_expression(parser, ctx));

        if (!ref_node)
        {
            zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_INTERNAL, "Could not initialize a reference node");
            return NULL;
        }

        if (!ref_node->expression)
        {
            zenit_node_free((struct ZenitNode*)ref_node);
            return NULL;
        }

        zenit_type_copy(&ref_node->base.typeinfo, &ref_node->expression->typeinfo);
        ref_node->base.typeinfo.is_ref = true;

        return (struct ZenitNode*)ref_node;
    }
    
    // Check for an identifier
    if (temp_token.type == ZENIT_TOKEN_ID)
    {
        zenit_parser_consume(parser);
        struct ZenitIdentifierNode *id_node = zenit_node_identifier_new(temp_token.location, token_to_string(&temp_token));

        if (!id_node)
        {
            zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_INTERNAL, "Could not initialize an identifier node");
            return NULL;
        }

        if (!id_node->name)
        {
            zenit_node_free((struct ZenitNode*)id_node);
            return NULL;
        }

        return (struct ZenitNode*)id_node;
    }

    // Try parsing a literal expression
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
 *  Parses a variable declaration
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 *  struct ZenitNode* - Variable declaration node
 * 
 * Grammar:
 *  variable_declaration = 'var' ID ( ':' type_information )? '=' expression ';' ;
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
        return NULL;
    }
    
    // Allocate the memory and the base information
    struct ZenitVariableNode *var_node = zenit_node_variable_new(temp_token.location, token_to_string(&temp_token));

    if (!var_node)
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_INTERNAL, "Could not initialize a variable node");
        return NULL;
    }

    // If the COLON token is present, we need to parse the type information
    if (zenit_parser_consume_if(parser, ZENIT_TOKEN_COLON) && !parse_typeinfo(parser, ctx, &var_node->base.typeinfo))
        goto missing_type;

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
    
    fl_free(var_node);

    return NULL;
}

/*
 * Function: parse_attribute_declaration
 *  This function parses a sequence of attributes declarations
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 * struct ZenitNode* - Parsed attribute declaration node
 * 
 * Grammar:
 *  attribute_declaration = '#' '[' ID ( '(' ID ':' expression ( ',' ID ':' expression )*  ')' )? ']' ;
 *
 */
static struct ZenitNode* parse_attribute_declaration(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    // Save the attribute's location
    struct ZenitToken hash_token;    

    if (!zenit_parser_expects(parser, ZENIT_TOKEN_HASH, &hash_token))
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Expecting token %s in attribute declaration, received %s", 
            zenit_token_print(ZENIT_TOKEN_HASH),
            zenit_token_print(zenit_parser_peek(parser).type));
        return NULL;
    }

    if (!zenit_parser_expects(parser, ZENIT_TOKEN_LBRACKET, NULL))
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Expecting token %s, received %s", 
            zenit_token_print(ZENIT_TOKEN_LBRACKET),
            zenit_token_print(zenit_parser_peek(parser).type));
        return NULL;
    }

    // Save the attribute name
    struct ZenitToken name_token;

    if (!zenit_parser_expects(parser, ZENIT_TOKEN_ID, &name_token))
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Expecting attribute name, received %s", 
            zenit_token_print(zenit_parser_peek(parser).type));
        return NULL;
    }
    
    // At this point we create the attribute node
    struct ZenitAttributeNode *attribute = zenit_node_attribute_new(hash_token.location, token_to_string(&name_token));

    if (attribute == NULL)
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_INTERNAL, "Could not initialize an attribute node");
        return NULL;
    }

    // If present, parse the attribute's properties
    if (zenit_parser_peek(parser).type == ZENIT_TOKEN_LPAREN)
    {
        zenit_parser_expects(parser, ZENIT_TOKEN_LPAREN, NULL);

        while (zenit_parser_peek(parser).type == ZENIT_TOKEN_ID)
        {
            // Consume the ID token with the property name
            struct ZenitToken prop_name = zenit_parser_consume(parser);

            if (!zenit_parser_expects(parser, ZENIT_TOKEN_COLON, NULL)) 
                goto parsing_error;

            // FIXME: We could restrict this to primitive types, by now it's ok
            struct ZenitNode *value = parse_expression(parser, ctx);

            if (value == NULL)
                goto parsing_error;

            // Create the property and add it to the attribute's properties map
            struct ZenitPropertyNode *property = zenit_node_property_new(prop_name.location, token_to_string(&prop_name), value);

            if (property != NULL)
            {
                zenit_property_node_map_add(&attribute->properties, property);
            }
            else
            {
                zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_INTERNAL, "Could not initialize a property node");
            }

            // Make sure there is a comma or a right parenthesis to continue parsing properties
            if (zenit_parser_peek(parser).type != ZENIT_TOKEN_RPAREN && !zenit_parser_expects(parser, ZENIT_TOKEN_COMMA, NULL))
                goto parsing_error;
        }

        zenit_parser_expects(parser, ZENIT_TOKEN_RPAREN, NULL);
    }

    if (!zenit_parser_expects(parser, ZENIT_TOKEN_RBRACKET, NULL))
        goto parsing_error;

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
 *  declaration = attribute_declaration | variable_declaration | statement ;
 *
 */
static struct ZenitNode* parse_declaration(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    // We save the next token to keep track of the location
    struct ZenitToken temp_token = zenit_parser_peek(parser);

    // Attributes may be present in multiple type of declarations (only variable by now)
    // therefore we need to parse them if present, and then handle if their usage is valid
    struct ZenitAttributeNodeMap attributes = zenit_attribute_node_map_new();

    while (zenit_parser_peek(parser).type == ZENIT_TOKEN_HASH)
    {
        struct ZenitAttributeNode *attribute = (struct ZenitAttributeNode*)parse_attribute_declaration(parser, ctx);

        if (attribute == NULL)
            continue;

        zenit_attribute_node_map_add(&attributes, attribute);
    }

    // Check for variables, functions, etc
    if (zenit_parser_peek(parser).type == ZENIT_TOKEN_VAR)
    {
        struct ZenitVariableNode *vardecl = (struct ZenitVariableNode*)parse_variable_declaration(parser, ctx);

        // Something happened if vardecl is NULL, we need to free the memory for the attribute map and leave
        if (vardecl == NULL)
        {
            zenit_attribute_node_map_free(&attributes);
            return NULL;
        }

        // Assign the attribute map (could be empty)
        vardecl->attributes = attributes;

        return (struct ZenitNode*)vardecl;
    }

    // If the attribute map is not empty at this point, it means their usage is invalid. Otherwise
    // we delete the map
    if (zenit_attribute_node_map_length(&attributes) > 0)
        zenit_context_error(ctx, temp_token.location, ZENIT_ERROR_SYNTAX, "Invalid use of attributes");
    else
        zenit_attribute_node_map_free(&attributes);

    // If there are no variables or functions declarations, it is a statement
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
    // final AST object
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
