#include <fllib.h>
#include "ast/ast.h"
#include "context.h"
#include "program.h"
#include "source.h"
#include "type.h"

/*
 * Function: error_free
 *  Frees the memory of an error object allocated with the <zenit_context_error> function
 *
 * Parameters:
 *  errorptr - Pointer to a <struct ZenitError> object
 *
 * Returns:
 *  void - This function does not return a value
 *
 */
static void error_free(void *errorptr)
{
    if (!errorptr)
        return;

    struct ZenitError *error = (struct ZenitError*)errorptr;

    if (error->message)
        fl_cstring_free(error->message);
}


/*
 * Function: zenit_context_new
 *  Allocates memory for a <struct ZenitSymtable> object and a <struct ZenitSourceInfo> object
 */
struct ZenitContext zenit_context_new(enum ZenitSourceType type, const char *input)
{
    struct ZenitContext ctx = { 
        .program = zenit_program_new(),
        .srcinfo = zenit_source_new(type, input),
        .errors = NULL
    };

    return ctx;
}

/*
 * Function: zenit_context_free
 *  Releases all the memory allocated by the <zenit_context_new> function
 *  but also if present, this function releases the memory of the <struct ZenitAst>
 *  object.
 */
void zenit_context_free(struct ZenitContext *ctx)
{
    if (!ctx)
        return;

    zenit_program_free(ctx->program);

    if (ctx->srcinfo)
        zenit_source_free(ctx->srcinfo);

    if (ctx->ast)
        zenit_ast_free(ctx->ast);

    if (ctx->errors)
        fl_array_free_each(ctx->errors, error_free);
}

/*
 * Function: zenit_context_error
 *  Initializes the *errors* property if needed and allocates memory for the formatted string. Both objects are
 *  deallocated by the <error_free> function.
 */
void zenit_context_error(struct ZenitContext *ctx, struct ZenitSourceLocation location, enum ZenitErrorType type, const char *message, ...)
{
    if (message == NULL)
        return;

    if (ctx->errors == NULL)
        ctx->errors = fl_array_new(sizeof(struct ZenitError), 0);

    va_list args;
    va_start(args, message);
    size_t length = vsnprintf(NULL, 0, message, args);
    va_end(args);

    if (length == 0)
    {
        return;
    }

    char *formatted_msg = fl_cstring_new(length);
    
    va_start(args, message);
    vsnprintf(formatted_msg, length+1, message, args);
    va_end(args);

    struct ZenitError error = {
        .type = type,
        .location = location,
        .message = formatted_msg
    };

    ctx->errors = fl_array_append(ctx->errors, &error);
}

void zenit_context_print_errors(struct ZenitContext *ctx)
{
    if (!zenit_context_has_errors(ctx))
        return;

    for (size_t i=0; i < fl_array_length(ctx->errors); i++)
    {
        fprintf(stderr, "%s:%d:%d: %s\n", 
            ctx->srcinfo->location.filename, 
            ctx->errors[i].location.line, 
            ctx->errors[i].location.col, 
            ctx->errors[i].message
        );
    }
}
