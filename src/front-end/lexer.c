#include <fllib.h>
#include "lexer.h"

/*
 * Macro: is_number
 *  Checks if *chr* is an ASCII number
 *
 * Parameters:
 *  chr - Char
 *
 */
#define is_number(chr) ((chr) >= 0x30 && (chr) <= 0x39)

/*
 * Macro: is_alpha
 *  Checks if *chr* is a letter
 *
 * Parameters:
 *  chr - Char.
 *
 */
#define is_alpha(chr) (((chr) >= 0x41 && (chr) <= 0x5A) || ((chr) >= 0x61 && (chr) <= 0x7A))

/*
 * Macro: is_reserved_keyword
 *  Checks if the <struct FlSlice> object *seq* is equals to the array of <FlByte>s represented
 *  by the string *str*
 *
 * Parameters:
 *  seq - Pointer to a <struct FlSlice> object.
 *  str - String.
 *
 */
#define is_reserved_keyword(seq, str) fl_slice_equals_sequence(seq, (FlByte*)(str), strlen(str))

/*
 * Macro: is_string
 *  Checks if the <struct FlSlice> object *seq* is equals to the string *str*
 *
 * Parameters:
 *  seq - Pointer to a <struct FlSlice> object.
 *  str - String.
 *
 */
#define is_string(seq, str) fl_slice_equals_sequence(seq, (FlByte*)(str), strlen(str))

/* Private API */

/*
 * Function: has_input
 *  Returns *true* if there is still input to process
 *
 * Parameters:
 *  lexer - Lexer object
 *
 * Returns:
 *  bool - *true* if there is input pending to process
 *
 */
static inline bool has_input(CenitLexer *lexer)
{
    return lexer->index < lexer->source.length;
}

/*
 * Function: peek
 *  Returns the current character pointed by the lexer, without
 *  actually consuming it
 *
 * Parameters:
 *  lexer - Lexer object
 *
 * Returns:
 *  char - Current character pointed by the lexer's internal pointer
 *
 */
static inline char peek(CenitLexer *lexer)
{
    return lexer->source.sequence[lexer->index];
}

/*
 * Function: peek_at
 *  Similar to <peek> but returning an element that is *index* elements
 *  ahead in the source. If the *index* "falls" outside of the valid range
 *  this function returns the NULL character ('\0')
 *
 * Parameters:
 *  lexer - Lexer object
 *  index - Offset
 *
 * Returns:
 *  char - Char placed *index* elements ahead of the lexer's internal pointer
 *          or NULL if *index* falls outside of the valid range.
 *
 */
static inline char peek_at(CenitLexer *lexer, size_t index)
{
    if (lexer->index + index >= lexer->source.length)
        return '\0';

    return lexer->source.sequence[lexer->index + index];
}

/*
 * Function: peek_many
 *  Returns a slice that takes *n* characters starting from the current internal
 *  pointer position.
 *  If *n* takes elements that fall outside of the valid range, the whole sequence
 *  is NULL to indicate that is not possible to peek that much number of characters
 *
 * Parameters:
 *  lexer - Lexer object
 *  n - Elements to take from the current internal pointer.
 *
 * Returns:
 *  struct FlSlice - Slice of characters
 *
 */
static inline struct FlSlice peek_many(CenitLexer *lexer, size_t n)
{
    if (lexer->index + n >= lexer->source.length)
        return (struct FlSlice){ .sequence = NULL };

    return fl_slice_new(lexer->source.sequence, 1, lexer->index, n);
}

/*
 * Function: peek_many_at
 *  Returns a slice that takes *n* characters starting from the current internal
 *  pointer position.
 *  If *n* takes elements that fall outside of the valid range, the whole sequence
 *  is NULL to indicate that is not possible to peek that much number of characters
 *
 * Parameters:
 *  lexer - Lexer object
 *  offset - Number of elements to skip
 *  n - Elements to take from the current internal pointer.
 *
 * Returns:
 *  struct FlSlice - Slice of characters
 *
 */
static inline struct FlSlice peek_many_at(CenitLexer *lexer, size_t offset, size_t n)
{
    if (lexer->index + offset + n >= lexer->source.length)
        return (struct FlSlice){ .sequence = NULL };

    return fl_slice_new(lexer->source.sequence, 1, lexer->index + offset, n);
}

/*
 * Function: consume
 *  Returns the current character and increments the internal pointer
 *
 * Parameters:
 *  lexer - Lexer object
 *
 * Returns:
 *  char - Character that has been consumed
 *
 */
static inline char consume(CenitLexer *lexer)
{
    lexer->col++;
    return lexer->source.sequence[lexer->index++];
}

/*
 * Variable: sync_char
 *  Group of synchronization characters of length 1
 *
 */
static char sync_char[] = { '\0', '\n', ' ', '\t', '\r', '#', ';', '=' };

/*
 * Variable: sync_chars
 *  Group of synchronization characters of length greater than 1
 *
 */
static char *sync_chars[] = { "//", "/*" };

/*
 * Function: is_sync_character
 *  Returns *true* if a char or a group of chars placed at *offset* elements of
 *  the current pointer location is *synchronization character* when an error
 *  occurs
 *
 * Parameters:
 *  lexer - Lexer object
 *  offset - Number of elements to skip from the current pointer location
 *
 * Returns:
 *  bool - *true* if the character (or group of chars) is a synchronization character
 *
 */
static inline bool is_sync_character(CenitLexer *lexer, size_t offset)
{
    char c = peek_at(lexer, offset);

    for (size_t i=0; i < sizeof(sync_char) / sizeof(sync_char[0]); i++)
        if (sync_char[i] == c)
            return true;

    // Comments
    struct FlSlice cc = peek_many_at(lexer, offset, 2);

    if (!cc.sequence)
        return false;

    for (size_t i=0; i < sizeof(sync_chars) / sizeof(sync_chars[0]); i++)
        if (is_string(&cc, sync_chars[i]))
            return true;

    return false;
}

/*
 * Function: remove_ws_and_comments
 *  This function strips out all the following white spaces and comments until
 *  fetching something that doesn't fall in that category
 *
 * Parameters:
 *  lexer - Lexer object
 *
 * Returns:
 *  void - This function does not return a value
 *
 */
static inline void remove_ws_and_comments(CenitLexer *lexer)
{
    while(has_input(lexer))
    {
        char c = peek(lexer);

        // New lines
        if (c == '\n')
        {
            consume(lexer);
            lexer->line++;
            lexer->col = 1;
            continue;
        }
        
        // Consume whitespace
        if (c == ' ' || c == '\t' || c == '\r')
        {
            consume(lexer);
            continue;
        }

        // Consume comments
        struct FlSlice cc = peek_many(lexer, 2);
        
        if (c == '#' || (cc.sequence && is_string(&cc, "//")))
        {
            // Consume comment line
            while (has_input(lexer) && peek(lexer) != '\n')
                consume(lexer);

            // Try again
            continue;
        }
        else if (cc.sequence && is_string(&cc, "/*"))
        {
            // Consume comment line
            while (has_input(lexer))
            {
                char c = consume(lexer);
                if (c == '*' && peek(lexer) == '/')
                {
                    consume(lexer);
                    break;
                }
                if (c == '\n')
                {
                    lexer->line++;
                    lexer->col = 1;
                }
            }

            // Try again
            continue;
        }

        // No ws nor comments, just break
        break;
    }
}

/*
 * Function: create_token
 *  Creates a new token object that starts at the lexer's internal pointer position
 *  and the length is determined by the *chars* variable. This function advances the pointer
 *  (consumes) *chars* time.
 *
 * Parameters:
 *  lexer - Lexer object
 *  type - Token's type.
 *  chars - Number of characters to consume as part of the token.
 *  line - Token's line number.
 *  col - Token's column number.
 *
 * Returns:
 *  CenitToken - Token object
 *
 */
static inline CenitToken create_token(CenitLexer *lexer, CenitTokenType type, size_t chars, unsigned int line, unsigned int col)
{
    // Current pointer position
    FlByte *starts = (FlByte*)lexer->source.sequence + lexer->index;    

    CenitToken token = { 
        .type = type,
        .value = fl_slice_new(starts, sizeof(char), 0, chars),
        .line = line,
        .col = col
    };

    // Advance the pointer as much as needed
    for (size_t i=0; i < chars; i++)
        consume(lexer);

    return token;
}

/* Public API */

CenitLexer cenit_lexer_new(const char *source)
{
    return (CenitLexer) {
        .source = fl_slice_new((const FlByte*)source, 1, 0, strlen(source)),
        .index = 0,
        .line = 1,
        .col = 1,
        .has_errors = false
    };
}

CenitToken* cenit_lexer_tokenize(CenitLexer *lexer)
{
    FlVector tempvec = fl_vector_new_args((struct FlVectorArgs) {
        .writer = fl_container_writer,
        .element_size = sizeof(CenitToken),
        .capacity = 1000
    });

    while (has_input(lexer))
    {
        CenitToken token = cenit_lexer_consume(lexer);

        if (token.type == CENIT_TOKEN_EOF)
            break;

        if (token.type == CENIT_TOKEN_UNKNOWN)
            lexer->has_errors = true;

        fl_vector_add(tempvec, &token);

        remove_ws_and_comments(lexer);
    }

    CenitToken* tokens = fl_vector_to_array(tempvec);

    fl_vector_free(tempvec);

    return tokens;
}

CenitToken cenit_lexer_consume(CenitLexer *lexer)
{
    remove_ws_and_comments(lexer);

    if (!has_input(lexer))
        return (CenitToken){ .type = CENIT_TOKEN_EOF };

    while (has_input(lexer))
    {
        char c = peek(lexer);

        if (c == ';')
        {
            return create_token(lexer, CENIT_TOKEN_SEMICOLON, 1, lexer->line, lexer->col);
        }
        else if (c == ',')
        {
            return create_token(lexer, CENIT_TOKEN_COMMA, 1, lexer->line, lexer->col);
        }
        else if (c == ':')
        {
            return create_token(lexer, CENIT_TOKEN_COLON, 1, lexer->line, lexer->col);
        }
        else if (c == '=')
        {
            return create_token(lexer, CENIT_TOKEN_ASSIGNMENT, 1, lexer->line, lexer->col);
        }
        else if (c == '{')
        {
            return create_token(lexer, CENIT_TOKEN_LBRACE, 1, lexer->line, lexer->col);
        }
        else if (c == '}')
        {
            return create_token(lexer, CENIT_TOKEN_RBRACE, 1, lexer->line, lexer->col);
        }
        else if (c == '[')
        {
            return create_token(lexer, CENIT_TOKEN_LBRACKET, 1, lexer->line, lexer->col);
        }
        else if (c == ']')
        {
            return create_token(lexer, CENIT_TOKEN_RBRACKET, 1, lexer->line, lexer->col);
        }
        else if (is_number(c))
        {
            // Take as much numbers as possible
            // TODO: Hex and binary support (prefix "0x" and suffix "b")
            size_t digits = 1;   
            while (is_number(peek_at(lexer, digits)))
                digits++;

            return create_token(lexer, CENIT_TOKEN_INTEGER, digits, lexer->line, lexer->col);
        }
        else if (is_alpha(c))
        {
            size_t chars = 1;
            while (is_number(peek_at(lexer, chars)) || is_alpha(peek_at(lexer, chars)) || peek_at(lexer, chars) == '_' || peek_at(lexer, chars) == '-')
                chars++;

            CenitToken token = create_token(lexer, CENIT_TOKEN_ID, chars, lexer->line, lexer->col);

            if (is_reserved_keyword(&token.value, "var"))
                token.type = CENIT_TOKEN_VAR;

            return token;
        }
        else
        {
            break;
        }
    }

    // If we reach this point, the token is unrecognized by the lexer
    // so we look for a synchronizing character and we return a UNKNOWN
    // token with the whole content
    lexer->has_errors = true;

    size_t sync_chars = 0;
    while (!is_sync_character(lexer, sync_chars))
        sync_chars++;

    return create_token(lexer, CENIT_TOKEN_UNKNOWN, sync_chars, lexer->line, lexer->col);
}

CenitToken cenit_lexer_peek(CenitLexer *lexer)
{
    // Save the lexer state
    unsigned int index = lexer->index;
    unsigned int line = lexer->line;
    unsigned int col = lexer->col;
    bool has_errors = lexer->has_errors;

    // Get the next token
    // TODO: We should buffer consumed tokens, by now
    // for simplicity it is ok to use this
    CenitToken token = cenit_lexer_consume(lexer);
    
    // Restore the lexer state
    lexer->index = index;
    lexer->line = line;
    lexer->col = col;
    lexer->has_errors = has_errors;

    return token;
}
