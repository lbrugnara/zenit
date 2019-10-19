#include <fllib.h>
#include "ast.h"
#include "context.h"
#include "type.h"

/*
 * Function: error_free
 *  Frees the memory of an error object allocated with the <parser_error> function
 *
 * Parameters:
 *  errorptr - Pointer to a <CenitError> object
 *
 * Returns:
 *  void - This function does not return a value
 *
 */
static void error_free(void *errorptr)
{
    if (!errorptr)
        return;

    CenitError *error = (CenitError*)errorptr;

    if (error->message)
        fl_cstring_free(error->message);
}


CenitContext cenit_context_new(const char *id)
{
    return (CenitContext) { 
        .symtable = cenit_symtable_new(CENIT_SYMTABLE_GLOBAL, id),
        .errors = NULL
    };
}

void cenit_context_free(CenitContext *ctx)
{
    if (!ctx)
        return;

    cenit_symtable_free(&ctx->symtable);

    if (ctx->ast)
        cenit_ast_free(ctx->ast);

    if (ctx->errors)
        fl_array_free_each(ctx->errors, error_free);
}

void cenit_context_error(CenitContext *ctx, CenitErrorType type, unsigned int line, unsigned int col, const char *message, ...)
{
    if (message == NULL)
        return;

    if (ctx->errors == NULL)
        ctx->errors = fl_array_new(sizeof(CenitError), 0);

    va_list args;
    va_start(args, message);
    
    size_t length = vsnprintf(NULL, 0, message, args);

    if (length == 0)
    {
        va_end(args);
        return;
    }

    char *formatted_msg = fl_cstring_new(length);
    
    vsnprintf(formatted_msg, length+1, message, args);
    va_end(args);

    CenitError error = {
        .type = type,
        .line = line,
        .col = col,
        .message = formatted_msg
    };

    ctx->errors = fl_array_append(ctx->errors, &error);
}
