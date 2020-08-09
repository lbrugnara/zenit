
#include <errno.h>
#include <limits.h>
#include "parse.h"
#include "../lexer.h"
#include "../parser.h"
#include "../ast/ast.h"
#include "../types/context.h"

#define consume_or_return(ctx, parser, token_type, tokenptr)                    \
    if (!zenit_parser_expects(parser, token_type, tokenptr))                    \
    {                                                                           \
        ZenitToken tmp;                                                  \
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
        ZenitToken tmp;                                                  \
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
        ZenitToken tmp;                                                  \
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

/* Private API */
static ZenitNode* parse_integer_literal(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_literal_expression(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_array_literal(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_reference_expression(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_identifier(ZenitParser *parser, ZenitContext *ctx, ZenitToken *id_token);
static ZenitNode* parse_struct_field(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_struct_literal(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_named_struct_literal(ZenitParser *parser, ZenitContext *ctx, ZenitToken *id_token);
static ZenitNode* parse_identifier_expression(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_unary_expression(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_cast_expression(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_expression(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_expression_statement(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_if_statement(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_block(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_statement(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_variable_declaration(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_field_declaration(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_struct_declaration(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_attribute_declaration(ZenitParser *parser, ZenitContext *ctx);
static ZenitNode* parse_declaration(ZenitParser *parser, ZenitContext *ctx);
static ZenitTypeNode* parse_type_declaration(ZenitParser *parser, ZenitContext *ctx, bool allow_partial_types);
static ZenitTypeNode* parse_type_array_declaration(ZenitParser *parser, ZenitContext *ctx, bool allow_partial_types);
static ZenitTypeNode* parse_type_reference_declaration(ZenitParser *parser, ZenitContext *ctx, bool allow_partial_types);
static bool parse_uint_value(ZenitContext *ctx, ZenitToken *primitive_token, ZenitUintTypeSize *size, ZenitUintValue *value);
static inline bool synchronize(ZenitParser *parser, ZenitTokenType *tokens, size_t length);

/*
 * Function: synchronize
 *  This function consumes tokens up to find a token that matches its type with one of the provided tokens
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitTokenType> *tokens: Array of synchronization tokens
 *  <size_t> length: Number of elements in the array
 *
 * Returns:
 *  bool: If the synchronization function finds one of the provided tokens, this function returns *true*, 
 *        otherwise it returns *false*.
 *
 * Notes:
 *  This function consume the matching token too.
 */
static inline bool synchronize(ZenitParser *parser, ZenitTokenType *tokens, size_t length)
{
    while (zenit_parser_has_input(parser))
    {
        ZenitToken token = zenit_parser_consume(parser);

        for (size_t i=0; i < length; i++)
            if (token.type == tokens[i])
                return true;
    }

    return false;
}

static ZenitTypeNode* parse_type_array_declaration(ZenitParser *parser, ZenitContext *ctx, bool allow_partial_types)
{
    ZenitToken bracket_token;
    consume_or_return(ctx, parser, ZENIT_TOKEN_LBRACKET, &bracket_token);

    bool auto_length = true;
    size_t length = 0;
    
    if (zenit_parser_next_is(parser, ZENIT_TOKEN_INTEGER))
    {
        // Size information must be an integer
        ZenitToken integer_token;
        consume_or_return(ctx, parser, ZENIT_TOKEN_INTEGER, &integer_token);    
        
        ZenitUintTypeSize size = ZENIT_UINT_UNK;
        ZenitUintValue value;
        assert_or_return(ctx, parse_uint_value(ctx, &integer_token, &size, &value), ZENIT_ERROR_INTERNAL, NULL);

        auto_length = false;
        switch (size)
        {
            case ZENIT_UINT_8:
                length = (size_t) value.uint8;
                break;

            case ZENIT_UINT_16:
                length = (size_t) value.uint16;
                break;
            
            default:
                auto_length = true;
                return false;
        }
    }
    else if (!allow_partial_types)
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX,
            "The length of the array is required in this context.");
    }

    // The closing bracket is required
    consume_or_return(ctx, parser, ZENIT_TOKEN_RBRACKET, NULL);

    ZenitTypeNode *member_type = parse_type_declaration(parser, ctx, allow_partial_types);

    ZenitArrayTypeNode *array_type_decl = zenit_node_type_array_new(bracket_token.location, member_type);

    array_type_decl->auto_length = auto_length;
    array_type_decl->length = length;

    return (ZenitTypeNode*) array_type_decl;
}

static ZenitTypeNode* parse_type_reference_declaration(ZenitParser *parser, ZenitContext *ctx, bool allow_partial_types)
{
    ZenitToken amp_token;
    consume_or_return(ctx, parser, ZENIT_TOKEN_AMPERSAND, &amp_token);

    ZenitTypeNode *element_type = parse_type_declaration(parser, ctx, allow_partial_types);

    return (ZenitTypeNode*) zenit_node_type_reference_new(amp_token.location, element_type);
}

/*
 * Function: parse_type_declaration
 *  Helper function that parses the type information and returns a <ZenitTypeNode> pointer
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  bool - If the type is present and is valid this function returns *true*. Otherwise returns *false*.
 *
 * Grammar:
 *  type_information = ( '[' integer_literal ']' )? '&'? ID
 */
static ZenitTypeNode* parse_type_declaration(ZenitParser *parser, ZenitContext *ctx, bool allow_partial_types)
{
    // Check for a reference
    if (zenit_parser_next_is(parser, ZENIT_TOKEN_AMPERSAND))    
        return parse_type_reference_declaration(parser, ctx, allow_partial_types);

    // Check if it is an array
    if (zenit_parser_next_is(parser, ZENIT_TOKEN_LBRACKET))
        return parse_type_array_declaration(parser, ctx, allow_partial_types);

    // Check for a reference
    if (zenit_parser_next_is(parser, ZENIT_TOKEN_AMPERSAND))    
        return parse_type_reference_declaration(parser, ctx, allow_partial_types);

    // The type name is required, so let's get it
    ZenitToken type_token;
    consume_or_return_val(ctx, parser, ZENIT_TOKEN_ID, &type_token, false);

    ZenitTypeKind zenit_type = zenit_type_from_slice(&type_token.value);

    if (zenit_type == ZENIT_TYPE_UINT)
    {
        ZenitUintTypeSize size = zenit_type_uint_size_from_slice(&type_token.value);
        return (ZenitTypeNode*) zenit_node_type_uint_new(type_token.location, size);
    }
    else if (zenit_type == ZENIT_TYPE_BOOL)
    {
        return (ZenitTypeNode*) zenit_node_type_bool_new(type_token.location);
    }
    else if (zenit_type == ZENIT_TYPE_STRUCT)
    {
        return (ZenitTypeNode*) zenit_node_type_struct_new(type_token.location, token_to_string(&type_token));
    }
    
    zenit_context_error(ctx, type_token.location, ZENIT_ERROR_INTERNAL, "Unhandled type");

    return NULL;
}

static bool parse_uint_value(ZenitContext *ctx, ZenitToken *primitive_token, ZenitUintTypeSize *size, ZenitUintValue *value)
{
    // This is the integer parsing logic (unsigned integers by now)
    char *endptr;
    short base = 10;
    unsigned long long temp_int = 0;
    
    // If it starts with 0x it is a hex value
    if (primitive_token->value.sequence[0] == '0' && primitive_token->value.sequence[1] == 'x')
        base = 16;

    errno = 0;
    temp_int = strtoull((const char*)primitive_token->value.sequence, &endptr, base);
    
    // The token length must be equals to the parsed number
    assert_or_return_val(ctx, (void*)(primitive_token->value.sequence + primitive_token->value.length) == (void*)endptr, ZENIT_ERROR_INTERNAL, 
        "Could not parse number", false);

    // Check for specific strtoull errors
    assert_or_return_val(ctx, (temp_int != ULLONG_MAX || errno != ERANGE) && (temp_int != 0ULL || errno != EINVAL), ZENIT_ERROR_LARGE_INTEGER, 
        "Integral value is too large", false);

    if (temp_int <= UINT8_MAX)
    {
        *size = ZENIT_UINT_8;
        value->uint8 = (uint8_t)temp_int;
    }
    else if (temp_int <= UINT16_MAX)
    {
        *size = ZENIT_UINT_16;
        value->uint16 = (uint16_t)temp_int;
    }
    else
    {
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_LARGE_INTEGER, "Integral value is too large");
        return false;
    }

    return true;
}

/*
 * Function: parse_integer_literal
 *  Parses an integer literal
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - The parsed integer literal
 *
 * Grammar:
 *  integer_literal = INTEGER
 */
static ZenitNode* parse_integer_literal(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitToken number_token;
    consume_or_return(ctx, parser, ZENIT_TOKEN_INTEGER, &number_token);    

    ZenitUintTypeSize size = ZENIT_UINT_UNK;
    ZenitUintValue value;
    assert_or_return(ctx, parse_uint_value(ctx, &number_token, &size, &value), ZENIT_ERROR_INTERNAL, NULL);

    ZenitUintNode *primitive_node = zenit_node_uint_new(number_token.location, size, value);

    assert_or_return(ctx, primitive_node != NULL, ZENIT_ERROR_INTERNAL, NULL);

    // Success
    return (ZenitNode*) primitive_node;
}

/*
 * Function: parse_boolean_literal
 *  Parses a boolean literal
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - The parsed boolean literal
 *
 * Grammar:
 *  boolean_literal = BOOL
 */
static ZenitNode* parse_boolean_literal(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitToken bool_token;
    consume_or_return(ctx, parser, ZENIT_TOKEN_BOOL, &bool_token);

    bool value = fl_slice_equals_sequence(&bool_token.value, (const FlByte * const) "true", 4);

    return (ZenitNode*) zenit_node_bool_new(bool_token.location, value);
}

/*
 * Function: parse_literal_expression
 *  Parses all the literal expressions supported by the language
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
*  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - Node representing the literal expression
 *
 * Grammar:
 *  literal_expression = integer_literal | boolean_literal ;
 */
static ZenitNode* parse_literal_expression(ZenitParser *parser, ZenitContext *ctx)
{
    if (zenit_parser_next_is(parser, ZENIT_TOKEN_INTEGER))
        return parse_integer_literal(parser, ctx);

    if (zenit_parser_next_is(parser, ZENIT_TOKEN_BOOL))
        return parse_boolean_literal(parser, ctx);
    
    zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Unknown literal expression");
    return NULL;
}

/*
 * Function: parse_array_literal
 *  Parses an array initializer
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - The <ZenitArrayNode> object
 *
 * Grammar:
 *  array_literal = '[' ( expression ( ',' expression )* ','? )? ']' ;
 */
static ZenitNode* parse_array_literal(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitToken lbracket_token;
    consume_or_return(ctx, parser, ZENIT_TOKEN_LBRACKET, &lbracket_token);

    // Allocate memory for the array node and fill the basic information
    ZenitArrayNode *array = zenit_node_array_new(lbracket_token.location);

    assert_or_return(ctx, array != NULL, ZENIT_ERROR_INTERNAL, "Could not initialize an array initializer node");

    while (zenit_parser_has_input(parser) && !zenit_parser_next_is(parser, ZENIT_TOKEN_RBRACKET))
    {
        // Each element in the array initializer is an expression
        ZenitNode *expression = parse_expression(parser, ctx);

        // Something happened while parsing the element, we need to leave
        assert_or_goto(ctx, expression != NULL, ZENIT_ERROR_INTERNAL, NULL, on_bad_expression_value);

        // Add the node to the elements list
        zenit_node_array_add_child(array, expression);

        // If the next token IS NOT a right bracket, it MUST be a comma (even a trailing comma)
        if (!zenit_parser_next_is(parser, ZENIT_TOKEN_RBRACKET))
            consume_or_goto(ctx, parser, ZENIT_TOKEN_COMMA, NULL, on_bad_expression_value);
    }

    consume_or_goto(ctx, parser, ZENIT_TOKEN_RBRACKET, NULL, on_missing_bracket);

    // Return the parsed array initializer
    return (ZenitNode*)array;

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
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - Parsed reference expression
 * 
 * Grammar:
 *  reference_expression = '&' expression ;
 *
 */
static ZenitNode* parse_reference_expression(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitToken amp_token;
    consume_or_return(ctx, parser, ZENIT_TOKEN_AMPERSAND, &amp_token);

    ZenitNode *expression = parse_expression(parser, ctx);
    assert_or_return(ctx, expression != NULL, ZENIT_ERROR_INTERNAL, NULL);

    ZenitReferenceNode *reference = zenit_node_reference_new(amp_token.location, expression);
    assert_or_goto(ctx, reference != NULL, ZENIT_ERROR_INTERNAL, "Could not initialize a reference node", on_reference_new_error);

    // Success
    return (ZenitNode*) reference;

    // Errors...
    on_reference_new_error: zenit_node_free(expression);
    return NULL;
}

/*
 * Function: parse_identifier
 *  Parses an identifier
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - Parsed identifier
 * 
 * Grammar:
 *  identifier = ID ;
 *
 */
static ZenitNode* parse_identifier(ZenitParser *parser, ZenitContext *ctx, ZenitToken *id_token)
{
    ZenitIdentifierNode *identifier = zenit_node_identifier_new(id_token->location, token_to_string(id_token));

    assert_or_return(ctx, identifier != NULL, ZENIT_ERROR_INTERNAL, "Could not initialize an identifier node");
    assert_or_goto(ctx, identifier->name != NULL && identifier->name[0] != '\0', ZENIT_ERROR_INTERNAL, "Identifier name cannot be empty", on_error);

    // Success
    return (ZenitNode*) identifier;

    // Errors...
    on_error: zenit_node_free((ZenitNode*)identifier);

    return NULL;
}

/*
 * Function: parse_struct_field
 *  Parses a struct field object
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - Parsed struct field
 * 
 * Grammar:
 *  struct_field = ID ':' expression ;
 *
 */
static ZenitNode* parse_struct_field(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitToken field_name;
    consume_or_return(ctx, parser, ZENIT_TOKEN_ID, &field_name);
    consume_or_return(ctx, parser, ZENIT_TOKEN_COLON, NULL);

    ZenitNode *value = parse_expression(parser, ctx);

    assert_or_return(ctx, value != NULL, ZENIT_ERROR_INTERNAL, NULL);

    ZenitFieldNode *field_node = zenit_node_field_new(field_name.location, token_to_string(&field_name));

    assert_or_return(ctx, field_node != NULL, ZENIT_ERROR_INTERNAL, "Could not initialize struct field node");

    field_node->value = value;

    return (ZenitNode*) field_node;
}

/*
 * Function: parse_struct_literal
 *  Parses a struct literal object
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - Parsed struct literal
 * 
 * Grammar:
 *  struct_literal = '{' struct_field ( ',' struct_field )* '}' ;
 *
 */
static ZenitNode* parse_struct_literal(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitToken brace_token;
    consume_or_return(ctx, parser, ZENIT_TOKEN_LBRACE, &brace_token);

    ZenitStructNode *struct_node = zenit_node_struct_new(brace_token.location, NULL);

    assert_or_return(ctx, struct_node != NULL, ZENIT_ERROR_INTERNAL, "Could not initialize a struct node");

    while (zenit_parser_has_input(parser))
    {
        ZenitNode *field_node = parse_struct_field(parser, ctx);

        assert_or_goto(ctx, field_node != NULL, ZENIT_ERROR_INTERNAL, NULL, on_error);

        ((ZenitFieldNode*) field_node)->owner = (ZenitNode*) struct_node;
        struct_node->members = fl_array_append(struct_node->members, &field_node);

        // If next token is a right parenthesis, break the loop, we finished parsing the properties
        if (zenit_parser_next_is(parser, ZENIT_TOKEN_RBRACE))
            break;

        // At this point, the comma is mandatory on every iteration, if we don't find it, we sync
        // with the right brace
        if (!zenit_parser_next_is(parser, ZENIT_TOKEN_COMMA))
        {
            ZenitToken tmp;
            zenit_parser_peek(parser, &tmp);

            if (synchronize(parser, (ZenitTokenType[]) { ZENIT_TOKEN_RBRACE }, 1))
            {
                zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX,
                    "Expecting token %s, received %s",
                    zenit_token_print(ZENIT_TOKEN_COMMA),
                    zenit_token_print(tmp.type));
            }
            else
            {
                zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX,
                    "Expecting token %s, received %s",
                    zenit_token_print(ZENIT_TOKEN_RBRACE),
                    zenit_token_print(tmp.type));
            }

            goto on_error;
        }

        consume_or_goto(ctx, parser, ZENIT_TOKEN_COMMA, NULL, on_error);
    }

    consume_or_return(ctx, parser, ZENIT_TOKEN_RBRACE, NULL);
    
    // Success
    return (ZenitNode*) struct_node;

    // Errors...
    on_error: zenit_node_struct_free(struct_node);

    return NULL;
}

/*
 * Function: parse_named_struct_literal
 *  Parses a named struct literal object
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - Parsed struct literal
 * 
 * Grammar:
 *  named_struct_literal = ID struct_literal ;
 *
 */
static ZenitNode* parse_named_struct_literal(ZenitParser *parser, ZenitContext *ctx, ZenitToken *id_token)
{
    ZenitStructNode *struct_node = (ZenitStructNode*) parse_struct_literal(parser, ctx);

    assert_or_return(ctx, struct_node != NULL, ZENIT_ERROR_INTERNAL, NULL);

    struct_node->name = token_to_string(id_token);
    memcpy(&struct_node->base.location, &id_token->location, sizeof(id_token->location));

    return (ZenitNode*) struct_node;
}

/*
 * Function: parse_identifier_expression
 *  Parses an identifier expression
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - Parsed identifier expression
 * 
 * Grammar:
 *  identifier_expression = named_struct_literal | identifier ;
 *
 */
static ZenitNode* parse_identifier_expression(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitToken temp_token;
    consume_or_return(ctx, parser, ZENIT_TOKEN_ID, &temp_token);

    if (zenit_parser_next_is(parser, ZENIT_TOKEN_LBRACE))
        return parse_named_struct_literal(parser, ctx, &temp_token);

    return parse_identifier(parser, ctx, &temp_token);
}

/*
 * Function: parse_unary_expression
 *  Parses a unary expression
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - Parsed unary expression
 * 
 * Grammar:
 *  unary_expression = cast_expression | array_literal | reference_expression | identifier_expression | literal_expression ;
 *
 */
static ZenitNode* parse_unary_expression(ZenitParser *parser, ZenitContext *ctx)
{
    if (zenit_parser_next_is(parser, ZENIT_TOKEN_CAST))
        return parse_cast_expression(parser, ctx);

    if (zenit_parser_next_is(parser, ZENIT_TOKEN_LBRACKET))
        return parse_array_literal(parser, ctx);

    if (zenit_parser_next_is(parser, ZENIT_TOKEN_LBRACE))
        return parse_struct_literal(parser, ctx);

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
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - Parsed cast expression
 * 
 * Grammar:
 *  cast_expression = 'cast' '(' expression ( ':' type_information )? ')' ;
 *
 */
static ZenitNode* parse_cast_expression(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitToken cast_token;

    consume_or_return(ctx, parser, ZENIT_TOKEN_CAST, &cast_token);
    consume_or_return(ctx, parser, ZENIT_TOKEN_LPAREN, NULL);

    ZenitNode *expression = parse_expression(parser, ctx);
    assert_or_return(ctx, expression != NULL, ZENIT_ERROR_INTERNAL, NULL);

    ZenitCastNode *cast_node = zenit_node_cast_new(cast_token.location, expression, false);

    if (zenit_parser_consume_if(parser, ZENIT_TOKEN_COLON))
    {
        cast_node->type_decl = parse_type_declaration(parser, ctx, true);
        assert_or_goto(ctx, cast_node->type_decl != NULL, ZENIT_ERROR_SYNTAX, NULL, on_error);
    }

    consume_or_return(ctx, parser, ZENIT_TOKEN_RPAREN, NULL);

    // Success
    return (ZenitNode*)cast_node;

    on_error: zenit_node_cast_free(cast_node);

    return NULL;
}

/*
 * Function: parse_expression
 *  Parses a single expression
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - Parsed expression node
 * 
 * Grammar:
 *  expression = unary_expression ;
 *
 */
static ZenitNode* parse_expression(ZenitParser *parser, ZenitContext *ctx)
{
    return parse_unary_expression(parser, ctx);
}

/*
 * Function: parse_expression_statement
 *  An expression statement is a a expression followed by a semicolon
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - Parsed expression
 *
 * Grammar:
 *  expression_statement = expression ';' ;
 */
static ZenitNode* parse_expression_statement(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitNode *node = parse_expression(parser, ctx);

    consume_or_goto(ctx, parser, ZENIT_TOKEN_SEMICOLON, NULL, on_missing_semicolon);

    // Success
    return node;

    // Cleanup code for error conditions
    on_missing_semicolon:  zenit_node_free(node);

    return NULL;
}

/*
 * Function: parse_block
 *  This function parses a block that is a starting left brace token followed by a set of
 *  statements and ends with a right brace token
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - The parsed block statement
 * 
 * Grammar:
 *  block = '{' declaration*  '}'
 */
static ZenitNode* parse_block(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitToken lbrace_token;
    consume_or_return(ctx, parser, ZENIT_TOKEN_LBRACE, &lbrace_token);

    ZenitBlockNode *block_node = zenit_node_block_new(lbrace_token.location);

    while (zenit_parser_has_input(parser) && !zenit_parser_next_is(parser, ZENIT_TOKEN_RBRACE))
    {
        ZenitNode *statement = parse_declaration(parser, ctx);

        assert_or_goto(ctx, statement != NULL, ZENIT_ERROR_INTERNAL, NULL, on_error);

        zenit_node_block_add_statement(block_node, statement);
    }

    consume_or_goto(ctx, parser, ZENIT_TOKEN_RBRACE, NULL, on_error);

    return (ZenitNode*) block_node;

    on_error:    zenit_node_block_free(block_node);

    return NULL;
}

/*
 * Function: parse_if_statement
 *  This function parses an if statement with its then branch and possibly its else branch
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - The parsed if statement
 * 
 * Grammar:
 *  if_statement = 'if' '(' expression ')' block ( 'else' ( if_statement | block ) )
 */
static ZenitNode* parse_if_statement(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitToken if_token;
    consume_or_return(ctx, parser, ZENIT_TOKEN_IF, &if_token);

    consume_or_return(ctx, parser, ZENIT_TOKEN_LPAREN, NULL);
    ZenitNode *condition = parse_expression(parser, ctx);
    assert_or_return(ctx, condition != NULL, ZENIT_ERROR_INTERNAL, NULL);
    consume_or_return(ctx, parser, ZENIT_TOKEN_RPAREN, NULL);

    ZenitNode *then_branch = parse_block(parser, ctx);
    assert_or_goto(ctx, then_branch != NULL, ZENIT_ERROR_INTERNAL, NULL, on_then_branch_error);

    ZenitNode *else_branch = NULL;
    if (zenit_parser_next_is(parser, ZENIT_TOKEN_ELSE))
    {
        consume_or_goto(ctx, parser, ZENIT_TOKEN_ELSE, NULL, on_else_branch_error);
        
        if (zenit_parser_next_is(parser, ZENIT_TOKEN_IF))
        {
            else_branch = parse_if_statement(parser, ctx);
        }
        else
        {
            else_branch = parse_block(parser, ctx);
        }
        assert_or_goto(ctx, else_branch != NULL, ZENIT_ERROR_INTERNAL, NULL, on_else_branch_error);
    }

    ZenitIfNode *if_node = zenit_node_if_new(if_token.location, condition, then_branch, else_branch);
    assert_or_goto(ctx, if_node != NULL, ZENIT_ERROR_INTERNAL, "Could not create if node", on_if_node_error);

    return (ZenitNode*) if_node;

    on_if_node_error:       if (else_branch) 
                                zenit_node_free(else_branch);
    on_else_branch_error:   zenit_node_free(then_branch);
    on_then_branch_error:   zenit_node_free(condition);

    return NULL;
}

/*
 * Function: parse_statement
 *  This function parses multiple type of statements: conditionals,
 *  loops, blocks, expression statements, etc.
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - The parsed statement
 * 
 * Grammar:
 *  statement = expression_statement | if_statement | block ;
 */
static ZenitNode* parse_statement(ZenitParser *parser, ZenitContext *ctx)
{
    if (zenit_parser_next_is(parser, ZENIT_TOKEN_IF))
        return parse_if_statement(parser, ctx);

    if (zenit_parser_next_is(parser, ZENIT_TOKEN_LBRACE))
        return parse_block(parser, ctx);

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
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - Variable declaration node
 * 
 * Grammar:
 *  variable_declaration = 'var' ID ( ':' type_information )? '=' expression ';' ;
 *
 */
static ZenitNode* parse_variable_declaration(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitToken var_token;
    ZenitToken name_token;
    
    consume_or_return(ctx, parser, ZENIT_TOKEN_VAR, &var_token);
    
    // The variable name is required to be present and to be valid
    if (!zenit_parser_expects(parser, ZENIT_TOKEN_ID, &name_token))
    {
        if (zenit_parser_next_is(parser, ZENIT_TOKEN_UNKNOWN))
        {
            ZenitToken tmp;
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
    ZenitVariableNode *var_node = zenit_node_variable_new(var_token.location, token_to_string(&name_token));

    assert_or_return(ctx, var_node != NULL, ZENIT_ERROR_INTERNAL, "Could not initialize a variable node");

    // If the COLON token is present, we need to parse the type information
    if (zenit_parser_consume_if(parser, ZENIT_TOKEN_COLON))
    {
        var_node->type_decl = parse_type_declaration(parser, ctx, true);
        assert_or_goto(ctx, var_node->type_decl != NULL, ZENIT_ERROR_SYNTAX, NULL, on_error);
    }

    // We don't allow variables without initializers, the ASSIGN token is required
    consume_or_goto(ctx, parser, ZENIT_TOKEN_ASSIGN, NULL, on_error);

    // The right-hand side value is an expression
    var_node->rvalue = parse_expression(parser, ctx);

    // As noted before, the initialization expression is required
    assert_or_goto(ctx, var_node->rvalue != NULL, ZENIT_ERROR_INTERNAL, NULL, on_error);

    // The variable declaration ends with a semicolon
    consume_or_goto(ctx, parser, ZENIT_TOKEN_SEMICOLON, NULL, on_error);

    // Success
    return (ZenitNode*) var_node;

    // Cleanup code for error conditions
    on_error: zenit_node_variable_free(var_node);

    return NULL;
}

/*
 * Function: parse_field_declaration
 *  Parses a struct member declaration
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - Struct member declaration node
 * 
 * Grammar:
 *  field_declaration = ID ':' type_information ';' ;
 *
 */
static ZenitNode* parse_field_declaration(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitToken name_token;
    
    // The field name is required to be present and to be valid
    if (!zenit_parser_expects(parser, ZENIT_TOKEN_ID, &name_token))
    {
        if (zenit_parser_next_is(parser, ZENIT_TOKEN_UNKNOWN))
        {
            ZenitToken tmp;
            zenit_parser_peek(parser, &tmp);
            struct FlSlice slice = tmp.value;
            size_t length = slice.length;
            const char *str = (const char*)slice.sequence;
            zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "'%.*s' is not a valid identifier", length, str);
        }
        else
        {
            zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Expecting field name but received %s", zenit_token_print(name_token.type));
        }
        return NULL;
    }
    
    // Allocate the memory and the base information
    ZenitFieldDeclNode *field_node = zenit_node_field_decl_new(name_token.location, token_to_string(&name_token));

    assert_or_return(ctx, field_node != NULL, ZENIT_ERROR_INTERNAL, "Could not initialize a field declaration node");

    // Type info is mandatory
    consume_or_goto(ctx, parser, ZENIT_TOKEN_COLON, NULL, on_error);
    
    field_node->type_decl = parse_type_declaration(parser, ctx, false);
    assert_or_goto(ctx, field_node->type_decl != NULL, ZENIT_ERROR_SYNTAX, NULL, on_error);

    // The field declaration ends with a semicolon
    consume_or_goto(ctx, parser, ZENIT_TOKEN_SEMICOLON, NULL, on_error);

    // Success
    return (ZenitNode*) field_node;

    // Cleanup code for error conditions
    on_error: zenit_node_field_decl_free(field_node);

    return NULL;
}

/*
 * Function: parse_struct_declaration
 *  Parses a struct declaration
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 *  ZenitNode* - Struct declaration node
 * 
 * Grammar:
 *  struct_declaration = 'struct' ID '{' field_declaration+ '}';
 *
 */
static ZenitNode* parse_struct_declaration(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitToken struct_token;
    ZenitToken name_token;
    consume_or_return(ctx, parser, ZENIT_TOKEN_STRUCT, &struct_token);
    
    // The struct name is required to be present and to be valid
    if (!zenit_parser_expects(parser, ZENIT_TOKEN_ID, &name_token))
    {
        if (zenit_parser_next_is(parser, ZENIT_TOKEN_UNKNOWN))
        {
            ZenitToken tmp;
            zenit_parser_peek(parser, &tmp);
            struct FlSlice slice = tmp.value;
            size_t length = slice.length;
            const char *str = (const char*)slice.sequence;
            zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "'%.*s' is not a valid struct name", length, str);
        }
        else
        {
            zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "Missing struct name");
        }
        return NULL;
    }

    // Allocate the memory and the base information
    ZenitStructDeclNode *struct_node = zenit_node_struct_decl_new(struct_token.location, token_to_string(&name_token));

    assert_or_return(ctx, struct_node != NULL, ZENIT_ERROR_INTERNAL, "Could not initialize a struct declaration node");

    consume_or_goto(ctx, parser, ZENIT_TOKEN_LBRACE, NULL, on_error);

    while (zenit_parser_has_input(parser) && !zenit_parser_next_is(parser, ZENIT_TOKEN_RBRACE))
    {
        // Get the struct member
        ZenitNode *struct_field = parse_field_declaration(parser, ctx);

        // Something happened while parsing the element, we need to leave
        assert_or_goto(ctx, struct_field != NULL, ZENIT_ERROR_INTERNAL, NULL, on_error);

        ((ZenitFieldDeclNode*) struct_field)->owner = (ZenitNode*) struct_node;
        struct_node->members = fl_array_append(struct_node->members, &struct_field);
    }

    if (fl_array_length(struct_node->members) == 0)
        zenit_context_error(ctx, ctx->srcinfo->location, ZENIT_ERROR_SYNTAX, "A struct needs at least one member, none found.");

    consume_or_goto(ctx, parser, ZENIT_TOKEN_RBRACE, NULL, on_error);    
    
    return (ZenitNode*) struct_node;

    // Cleanup code for error conditions
    on_error: zenit_node_struct_decl_free(struct_node);

    return NULL;
}

/*
 * Function: parse_attribute_declaration
 *  This function parses a sequence of attributes declarations
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 * ZenitNode* - Parsed attribute declaration node
 * 
 * Grammar:
 *  attribute_declaration = '#' '[' ID ( '(' ID ':' expression ( ',' ID ':' expression )*  ')' )? ']' ;
 *
 */
static ZenitNode* parse_attribute_declaration(ZenitParser *parser, ZenitContext *ctx)
{
    // Save the attribute's location
    ZenitToken hash_token;
    // Save the attribute name
    ZenitToken name_token;

    consume_or_return(ctx, parser, ZENIT_TOKEN_HASH, &hash_token);
    consume_or_return(ctx, parser, ZENIT_TOKEN_LBRACKET, NULL);
    consume_or_return(ctx, parser, ZENIT_TOKEN_ID, &name_token);
    
    // At this point we create the attribute node
    ZenitAttributeNode *attribute = zenit_node_attribute_new(hash_token.location, token_to_string(&name_token));    

    assert_or_return(ctx, attribute != NULL, ZENIT_ERROR_INTERNAL, "Could not initialize an attribute node");

    // If present, parse the attribute's properties
    if (zenit_parser_consume_if(parser, ZENIT_TOKEN_LPAREN))
    {
        while (zenit_parser_next_is(parser, ZENIT_TOKEN_ID))
        {
            // Consume the ID token with the property name
            ZenitToken prop_name = zenit_parser_consume(parser);

            consume_or_goto(ctx, parser, ZENIT_TOKEN_COLON, NULL, on_parsing_error);

            // FIXME: We could restrict this to primitive types, by now it's ok
            ZenitNode *value = parse_expression(parser, ctx);

            assert_or_goto(ctx, value != NULL, ZENIT_ERROR_INTERNAL, NULL, on_parsing_error);

            // Create the property and add it to the attribute's properties map
            ZenitPropertyNode *property = zenit_node_property_new(prop_name.location, token_to_string(&prop_name), value);

            if (property != NULL)
            {
                zenit_property_node_map_add(attribute->properties, property);
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
    return (ZenitNode*)attribute;

    // FIXME: Implement the free function for attributes list
    on_parsing_error: zenit_node_free((ZenitNode*)attribute);

    return NULL;
}

/*
 * Function: parse_attribute_declaration_list
 *  Helper function that if possible, parses a sequence of attribute declarations
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 * ZenitAttributeNodeMap* - Map with the parsed attributes or an empty map
 *
 */
static ZenitAttributeNodeMap* parse_attribute_declaration_list(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitAttributeNodeMap *attributes = zenit_attribute_node_map_new();

    while (zenit_parser_next_is(parser, ZENIT_TOKEN_HASH))
    {
        ZenitAttributeNode *attribute = (ZenitAttributeNode*)parse_attribute_declaration(parser, ctx);

        if (attribute == NULL)
            continue;

        zenit_attribute_node_map_add(attributes, attribute);
    }

    return attributes;
}

/*
 * Function: parse_declaration
 *  This is the top-level non-terminal of the language
 *
 * Parameters:
 *  <ZenitParser> *parser: Parser object
 *  <ZenitContext> *ctx: Context object
 *
 * Returns:
 * ZenitNode* - Parsed declaration node
 * 
 * Grammar:
 *  declaration = attribute_declaration* ( variable_declaration | struct_declaration ) | statement ;
 *
 */
static ZenitNode* parse_declaration(ZenitParser *parser, ZenitContext *ctx)
{
    ZenitSourceLocation location = ctx->srcinfo->location;

    // We first check for attributes
    ZenitAttributeNodeMap *attributes = parse_attribute_declaration_list(parser, ctx);

    // Check for variables, functions, etc
    if (zenit_parser_next_is(parser, ZENIT_TOKEN_VAR))
    {
        ZenitVariableNode *vardecl = (ZenitVariableNode*) parse_variable_declaration(parser, ctx);

        // Something happened if vardecl is NULL, we need to free the memory for the attribute map and leave
        assert_or_goto(ctx, vardecl != NULL, ZENIT_ERROR_INTERNAL, NULL, on_parsing_error);

        // Assign the attribute map (could be empty)
        vardecl->attributes = attributes;

        return (ZenitNode*) vardecl;
    }
    else if (zenit_parser_next_is(parser, ZENIT_TOKEN_STRUCT))
    {
        ZenitStructDeclNode *struct_decl = (ZenitStructDeclNode*) parse_struct_declaration(parser, ctx);

        // Something happened if struct_decl is NULL, we need to free the memory for the attribute map and leave
        assert_or_goto(ctx, struct_decl != NULL, ZENIT_ERROR_INTERNAL, NULL, on_parsing_error);

        // Assign the attribute map (could be empty)
        struct_decl->attributes = attributes;

        return (ZenitNode*) struct_decl;
    }

    // If the attribute map is not empty at this point, it means their usage is invalid
    if (zenit_attribute_node_map_length(attributes) > 0)
        zenit_context_error(ctx, location, ZENIT_ERROR_SYNTAX, "Invalid use of attributes");
        
    // At this point we always free the attributes map, no one will use it
    zenit_attribute_node_map_free(attributes);

    // If there are no variables or functions declarations, it is a statement
    return parse_statement(parser, ctx);

    // Errors..
    on_parsing_error: zenit_attribute_node_map_free(attributes);

    return NULL;
}

/*
 * Function: zenit_parse_source
 *  While the parser has input, we call the <parse_declaration> function
 *  that is the top-level function that knows how to "traverse" the code
 */
bool zenit_parse_source(ZenitContext *ctx)
{
    FlList *templist = fl_list_new();
    size_t decls_count = 0;

    ZenitParser parser = zenit_parser_new(ctx->srcinfo);

    // Each iteration processes a declaration which is a subtree of the
    // final AST object
    while (zenit_parser_has_input(&parser))
    {
        ZenitNode *declaration = parse_declaration(&parser, ctx);

        // If declaration is NULL, something happened, so let's synch
        if (declaration == NULL)
        {
            if (!synchronize(&parser, (ZenitTokenType[]) { ZENIT_TOKEN_SEMICOLON, ZENIT_TOKEN_EOF }, 2))
                break;

            // Let's try again
            continue;
        }

        fl_list_append(templist, declaration);
        decls_count++;
    }

    // Create the decls array
    ZenitNode **decls = fl_array_new(sizeof(ZenitNode*), decls_count);

    decls_count = 0;
    struct FlListNode *node = fl_list_head(templist);
    while (node)
    {
        decls[decls_count++] = (ZenitNode*) node->value;
        node = node->next;
    }

    // Create the ZenitAst object
    ctx->ast = zenit_ast_new(decls);
    fl_list_free(templist);

    return !ctx->errors;
}
