#include <fllib.h>
#include <errno.h>
#include <limits.h>
#include "lexer.h"
#include "parse.h"
#include "parser.h"
#include "ast.h"

#define consume_or_return(ctx, parser, token_type, tokenptr)                    \
    if (!zenit_parser_expects(parser, token_type, tokenptr))                    \
    {                                                                           \
        struct ZenitToken tmp;                                                  \
        zenit_parser_peek(parser, &tmp);                                        \
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX,    \
            "Expecting token %s, received %s",                                  \
            zenit_token_print(token_type),                                      \
            zenit_token_print(tmp.type));                                       \
        return NULL;                                                            \
    }

#define consume_or_return_val(ctx, parser, token_type, tokenptr, retval)        \
    if (!zenit_parser_expects(parser, token_type, tokenptr))                    \
    {                                                                           \
        struct ZenitToken tmp;                                                  \
        zenit_parser_peek(parser, &tmp);                                        \
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX,    \
            "Expecting token %s, received %s",                                  \
            zenit_token_print(token_type),                                      \
            zenit_token_print(tmp.type));                                       \
        return retval;                                                          \
    }

#define consume_or_goto(ctx, parser, token_type, tokenptr, label)               \
    if (!zenit_parser_expects(parser, token_type, tokenptr))                    \
    {                                                                           \
        struct ZenitToken tmp;                                                  \
        zenit_parser_peek(parser, &tmp);                                        \
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX,    \
            "Expecting token %s, received %s",                                  \
            zenit_token_print(token_type),                                      \
            zenit_token_print(tmp.type));                                       \
        goto label;                                                             \
    }

#define assert_or_return(ctx, condition, error, message)                    \
        if (!(condition))                                                   \
        {                                                                   \
            if (message != NULL)                                            \
                zenit_context_error(ctx, ctx->srcinfo->location,            \
                    error, message);                                        \
            return NULL;                                                    \
        }

#define assert_or_return_val(ctx, condition, error, message, retval)        \
        if (!(condition))                                                   \
        {                                                                   \
            if (message != NULL)                                            \
                zenit_context_error(ctx, ctx->srcinfo->location,            \
                    error, message);                                        \
            return retval;                                                  \
        }

#define assert_or_goto(ctx, condition, error, message, label)               \
        if (!(condition))                                                   \
        {                                                                   \
            if (message != NULL)                                            \
                zenit_context_error(ctx, ctx->srcinfo->location,            \
                    error, message);                                        \
            goto label;                                                     \
        }

#define token_to_string(tokenptr)                                           \
    fl_cstring_dup_n(                                                       \
        (const char*)(tokenptr)->value.sequence,                            \
        (tokenptr)->value.length                                            \
    )

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

        // Parse the array size that must be an integer
        struct ZenitPrimitiveNode *primitive_node = (struct ZenitPrimitiveNode*)parse_integer_literal(parser, ctx);
        
        assert_or_return_val(ctx, primitive_node != NULL, ZENIT_ERROR_INTERNAL, NULL, false);

        // If the primitive_node node is valid, we get the size form its value
        switch (primitive_node->base.typeinfo.type)
        {
            case ZENIT_TYPE_UINT8:
                typeinfo->elements = (size_t)primitive_node->value.uint8;
                break;

            case ZENIT_TYPE_UINT16:
                typeinfo->elements = (size_t)primitive_node->value.uint16;
                break;
            
            default:
                return false;
        }

        // We can safely free the size here
        zenit_node_free((struct ZenitNode*)primitive_node);

        // The closing bracket is required
        consume_or_return_val(ctx, parser, ZENIT_TOKEN_RBRACKET, NULL, false);
    }
    else
    {
        // The type declaration is a single element
        typeinfo->is_array = false;
        typeinfo->elements = 1;
    }
    
    // Check if it is a reference
    typeinfo->is_ref = zenit_parser_consume_if(parser, ZENIT_TOKEN_AMPERSAND);

    // The type name is required, so let's get it
    struct ZenitToken type_token;
    consume_or_return_val(ctx, parser, ZENIT_TOKEN_ID, &type_token, false);

    // Fill the node's <struct ZenitTypeInfo> object
    fill_typeinfo_from_token(typeinfo, &type_token);

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
 *  integer_literal = INTEGER
 */
static struct ZenitNode* parse_integer_literal(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    struct ZenitToken number_token;
    consume_or_return(ctx, parser, ZENIT_TOKEN_INTEGER, &number_token);    

    // This is the integer parsing logic (unsigned integers by now)
    char *endptr;
    short base = 10;
    unsigned long long temp_int = 0;
    
    // If it starts with 0x it is a hex value
    if (number_token.value.sequence[0] == '0' && number_token.value.sequence[1] == 'x')
        base = 16;

    temp_int = strtoull((const char*)number_token.value.sequence, &endptr, base);
    
    // The token length must be equals to the parsed number
    assert_or_return(ctx, (void*)(number_token.value.sequence + number_token.value.length) == (void*)endptr, ZENIT_ERROR_INTERNAL, "Could not parse number");

    // Check for specific strtoull errors
    assert_or_return(ctx, (temp_int != ULLONG_MAX || errno != ERANGE) && (temp_int != 0ULL || errno != EINVAL), ZENIT_ERROR_LARGE_INTEGER, "Integral value is too large");


    struct ZenitPrimitiveNode *primitive_node = NULL;

    if (temp_int <= UINT8_MAX)
    {
        primitive_node = zenit_node_primitive_new(number_token.location, ZENIT_TYPE_UINT8, (union ZenitPrimitiveValue){ .uint8 = (uint8_t)temp_int });
    }
    else if (temp_int <= UINT16_MAX)
    {
        primitive_node = zenit_node_primitive_new(number_token.location, ZENIT_TYPE_UINT16, (union ZenitPrimitiveValue){ .uint16 = (uint16_t)temp_int });
    }

    assert_or_return(ctx, primitive_node != NULL, ZENIT_ERROR_LARGE_INTEGER, "Integral value is too large");

    // Success
    return (struct ZenitNode*)primitive_node;
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
    // By now we just parse integers
    if (zenit_parser_next_is(parser, ZENIT_TOKEN_INTEGER))
        return parse_integer_literal(parser, ctx);
    
    zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Unknown literal expression");
    return NULL;
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
    struct ZenitToken lbracket_token;

    consume_or_return(ctx, parser, ZENIT_TOKEN_LBRACKET, &lbracket_token);

    // Allocate memory for the array node and fill the basic information
    struct ZenitArrayNode *array = zenit_node_array_new(lbracket_token.location);

    assert_or_return(ctx, array != NULL, ZENIT_ERROR_INTERNAL, "Could not initialize an array initializer node");

    // Keep iterating until get an EOF token or breaking out from inside 
    while (zenit_parser_has_input(parser) && !zenit_parser_next_is(parser, ZENIT_TOKEN_RBRACKET))
    {
        // Each element in the array initializer is an expression
        struct ZenitNode *value = parse_expression(parser, ctx);

        // Something happened parsing the element, we need to leave
        assert_or_goto(ctx, value != NULL, ZENIT_ERROR_INTERNAL, NULL, on_bad_expression_value);

        array->elements = fl_array_append(array->elements, &value);

        // If the next token IS NOT a right bracket, it MUST be a comma (even a trailing comma)
        if (!zenit_parser_next_is(parser, ZENIT_TOKEN_RBRACKET))
            consume_or_goto(ctx, parser, ZENIT_TOKEN_COMMA, NULL, on_bad_expression_value);
    }

    // If the following token is not the right bracket, we reached the EOF token
    consume_or_goto(ctx, parser, ZENIT_TOKEN_RBRACKET, NULL, on_missing_bracket);

    // Update the number of elements
    array->base.typeinfo.elements = fl_array_length(array->elements);

    // Return the parsed array initializer
    return (struct ZenitNode*)array;

    // Cleanup code for error conditions
    on_bad_expression_value:
    on_missing_bracket:
        zenit_node_array_free(array);

    return NULL;
}

/*
 * Function: parse_reference_expression
 *  Parses a reference expression
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 *  struct ZenitNode* - Parsed reference expression
 * 
 * Grammar:
 *  reference_expression = '&' expression ;
 *
 */
static struct ZenitNode* parse_reference_expression(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    struct ZenitToken amp_token;
    consume_or_return(ctx, parser, ZENIT_TOKEN_AMPERSAND, &amp_token);

    struct ZenitNode *expression = parse_expression(parser, ctx);
    assert_or_return(ctx, expression != NULL, ZENIT_ERROR_INTERNAL, NULL);

    struct ZenitReferenceNode *reference = zenit_node_reference_new(amp_token.location, expression);
    assert_or_goto(ctx, reference != NULL, ZENIT_ERROR_INTERNAL, "Could not initialize a reference node", on_reference_new_error);

    reference->base.typeinfo.is_ref = true;
    zenit_type_copy(&reference->base.typeinfo, &reference->expression->typeinfo);

    return (struct ZenitNode*)reference;

    on_reference_new_error: zenit_node_free(expression);

    return NULL;
}

/*
 * Function: parse_identifier_expression
 *  Parses an identifier expression
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 *  struct ZenitNode* - Parsed identifier expression
 * 
 * Grammar:
 *  identifier_expression = ID ;
 *
 */
static struct ZenitNode* parse_identifier_expression(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    struct ZenitToken temp_token;
    consume_or_return(ctx, parser, ZENIT_TOKEN_ID, &temp_token);

    struct ZenitIdentifierNode *identifier = zenit_node_identifier_new(temp_token.location, token_to_string(&temp_token));

    assert_or_return(ctx, identifier != NULL, ZENIT_ERROR_INTERNAL, "Could not initialize an identifier node");
    assert_or_goto(ctx, identifier->name != NULL && identifier->name[0] != '\0', ZENIT_ERROR_INTERNAL, "Identifier name cannot be empty", on_error);

    // Success
    return (struct ZenitNode*)identifier;

    // Errors...
    on_error: zenit_node_free((struct ZenitNode*)identifier);

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
 *  unary_expression = reference_expression | identifier_expression | literal_expression ;
 *
 */
static struct ZenitNode* parse_unary_expression(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    // Check for a reference expression
    if (zenit_parser_next_is(parser, ZENIT_TOKEN_AMPERSAND))
        return parse_reference_expression(parser, ctx);
    
    // Check for an identifier
    if (zenit_parser_next_is(parser, ZENIT_TOKEN_ID))
        return parse_identifier_expression(parser, ctx);

    // Try parsing a literal expression
    return parse_literal_expression(parser, ctx);
}

/*
 * Function: parse_cast_expression
 *  Parses a cast expression
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 *  struct ZenitNode* - Parsed cast expression
 * 
 * Grammar:
 *  cast_expression = 'cast' '(' expression ( ':' type_information )? ')' ;
 *
 */
static struct ZenitNode* parse_cast_expression(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    struct ZenitToken cast_token;

    consume_or_return(ctx, parser, ZENIT_TOKEN_CAST, &cast_token);
    consume_or_return(ctx, parser, ZENIT_TOKEN_LPAREN, NULL);

    struct ZenitNode *expression = parse_expression(parser, ctx);
    assert_or_return(ctx, expression != NULL, ZENIT_ERROR_INTERNAL, NULL);

    struct ZenitCastNode *cast_node = zenit_node_cast_new(cast_token.location, expression, NULL, false);

    if (zenit_parser_consume_if(parser, ZENIT_TOKEN_COLON))
        assert_or_goto(ctx, parse_typeinfo(parser, ctx, &cast_node->base.typeinfo), ZENIT_ERROR_SYNTAX, NULL, on_error);

    consume_or_return(ctx, parser, ZENIT_TOKEN_RPAREN, NULL);

    // Success
    return (struct ZenitNode*)cast_node;

    on_error: zenit_node_cast_free(cast_node);

    return NULL;
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
 *  expression = cast_expression | array_initializer | unary_expression ;
 *
 */
static struct ZenitNode* parse_expression(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    if (zenit_parser_next_is(parser, ZENIT_TOKEN_CAST))
        return parse_cast_expression(parser, ctx);

    if (zenit_parser_next_is(parser, ZENIT_TOKEN_LBRACKET))
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

    consume_or_goto(ctx, parser, ZENIT_TOKEN_SEMICOLON, NULL, on_missing_semicolon);

    // Success
    return node;

    // Cleanup code for error conditions
    on_missing_semicolon:  zenit_node_free(node);

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
    struct ZenitToken var_token;
    struct ZenitToken name_token;
    
    consume_or_return(ctx, parser, ZENIT_TOKEN_VAR, &var_token);
    
    // The variable name is required to be present and be valid
    if (!zenit_parser_expects(parser, ZENIT_TOKEN_ID, &name_token))
    {
        if (zenit_parser_next_is(parser, ZENIT_TOKEN_UNKNOWN))
        {
            struct ZenitToken tmp;
            zenit_parser_peek(parser, &tmp);
            struct FlSlice slice = tmp.value;
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
    struct ZenitVariableNode *var_node = zenit_node_variable_new(var_token.location, token_to_string(&name_token));

    assert_or_return(ctx, var_node != NULL, ZENIT_ERROR_INTERNAL, "Could not initialize a variable node");

    // If the COLON token is present, we need to parse the type information
    if (zenit_parser_consume_if(parser, ZENIT_TOKEN_COLON))
        assert_or_goto(ctx, parse_typeinfo(parser, ctx, &var_node->base.typeinfo), ZENIT_ERROR_SYNTAX, NULL, on_missing_type);

    // We don't allow variables without initializers, the ASSIGN token is required
    consume_or_goto(ctx, parser, ZENIT_TOKEN_ASSIGN, NULL, on_missing_assignment);

    // The right-hand side value is an expression
    var_node->rvalue = parse_expression(parser, ctx);

    // As noted before, the initialization expression is required
    assert_or_goto(ctx, var_node->rvalue != NULL, ZENIT_ERROR_INTERNAL, NULL, on_missing_assignment);

    // The variable declaration ends with a semicolon
    consume_or_goto(ctx, parser, ZENIT_TOKEN_SEMICOLON, NULL, on_missing_semicolon);

    // Success
    return (struct ZenitNode*)var_node;

    // Cleanup code for error conditions
    on_missing_semicolon:   zenit_node_free(var_node->rvalue);
    on_missing_assignment:  fl_cstring_free(var_node->base.typeinfo.name);
    on_missing_type:        fl_cstring_free(var_node->name);
    
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
    // Save the attribute name
    struct ZenitToken name_token;

    consume_or_return(ctx, parser, ZENIT_TOKEN_HASH, &hash_token);
    consume_or_return(ctx, parser, ZENIT_TOKEN_LBRACKET, NULL);
    consume_or_return(ctx, parser, ZENIT_TOKEN_ID, &name_token);
    
    // At this point we create the attribute node
    struct ZenitAttributeNode *attribute = zenit_node_attribute_new(hash_token.location, token_to_string(&name_token));    

    assert_or_return(ctx, attribute != NULL, ZENIT_ERROR_INTERNAL, "Could not initialize an attribute node");

    // If present, parse the attribute's properties
    if (zenit_parser_consume_if(parser, ZENIT_TOKEN_LPAREN))
    {
        while (zenit_parser_next_is(parser, ZENIT_TOKEN_ID))
        {
            // Consume the ID token with the property name
            struct ZenitToken prop_name = zenit_parser_consume(parser);

            consume_or_goto(ctx, parser, ZENIT_TOKEN_COLON, NULL, on_parsing_error);

            // FIXME: We could restrict this to primitive types, by now it's ok
            struct ZenitNode *value = parse_expression(parser, ctx);

            assert_or_goto(ctx, value != NULL, ZENIT_ERROR_INTERNAL, NULL, on_parsing_error);

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

            // If next token is a right parenthesis, break the loop, we finished parsing the properties
            if (zenit_parser_next_is(parser, ZENIT_TOKEN_RPAREN))
                break;

            // At this point, the comma is mandatory on every iteration
            consume_or_goto(ctx, parser, ZENIT_TOKEN_COMMA, NULL, on_parsing_error);
        }

        consume_or_goto(ctx, parser, ZENIT_TOKEN_RPAREN, NULL, on_parsing_error);
    }

    consume_or_goto(ctx, parser, ZENIT_TOKEN_RBRACKET, NULL, on_parsing_error);

    // Success
    return (struct ZenitNode*)attribute;

    // FIXME: Implement the free function for attributes list
    on_parsing_error: zenit_node_free((struct ZenitNode*)attribute);

    return NULL;
}

/*
 * Function: parse_attribute_declaration_list
 *  Helper function that if possible, parses a sequence of attribute declarations
 *
 * Parameters:
 *  parser - Parser object
 *  ctx - <struct ZenitContext> object
 *
 * Returns:
 * struct ZenitAttributeNodeMap - Map with the parsed attributes or an empty map
 *
 */
static struct ZenitAttributeNodeMap parse_attribute_declaration_list(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    struct ZenitAttributeNodeMap attributes = zenit_attribute_node_map_new();

    while (zenit_parser_next_is(parser, ZENIT_TOKEN_HASH))
    {
        struct ZenitAttributeNode *attribute = (struct ZenitAttributeNode*)parse_attribute_declaration(parser, ctx);

        if (attribute == NULL)
            continue;

        zenit_attribute_node_map_add(&attributes, attribute);
    }

    return attributes;
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
 *  declaration = attribute_declaration* variable_declaration | statement ;
 *
 */
static struct ZenitNode* parse_declaration(struct ZenitParser *parser, struct ZenitContext *ctx)
{
    struct ZenitSourceLocation location = ctx->srcinfo->location;

    // We first check for attributes
    struct ZenitAttributeNodeMap attributes = parse_attribute_declaration_list(parser, ctx);

    // Check for variables, functions, etc
    if (zenit_parser_next_is(parser, ZENIT_TOKEN_VAR))
    {
        struct ZenitVariableNode *vardecl = (struct ZenitVariableNode*)parse_variable_declaration(parser, ctx);

        // Something happened if vardecl is NULL, we need to free the memory for the attribute map and leave
        assert_or_goto(ctx, vardecl != NULL, ZENIT_ERROR_INTERNAL, NULL, on_parsing_error);

        // Assign the attribute map (could be empty)
        vardecl->attributes = attributes;

        return (struct ZenitNode*)vardecl;
    }

    // If the attribute map is not empty at this point, it means their usage is invalid
    if (zenit_attribute_node_map_length(&attributes) > 0)
        zenit_context_error(ctx, location, ZENIT_ERROR_SYNTAX, "Invalid use of attributes");
        
    // At this point we always free the attributes map, no one will use it
    zenit_attribute_node_map_free(&attributes);

    // If there are no variables or functions declarations, it is a statement
    return parse_statement(parser, ctx);

    // Errors..
    on_parsing_error: zenit_attribute_node_map_free(&attributes);

    return NULL;
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
