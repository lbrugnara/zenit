#include <fllib.h>
#include "ast.h"
#include "context.h"
#include "source.h"
#include "type.h"

/*
 * Function: error_free
 *  Frees the memory of an error object allocated with the <cenit_context_error> function
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


/*
 * Function: cenit_context_new
 *  Allocates memory for a <CenitSymbolTable> object and a <CenitSourceInfo> object
 */
CenitContext cenit_context_new(const char *id, CenitSourceType type, const char *input)
{
    CenitContext ctx = { 
        .symtable = cenit_symtable_new(CENIT_SYMTABLE_GLOBAL, id),
        .srcinfo = cenit_source_new(type, input),
        .errors = NULL
    };

    return ctx;
}

/*
 * Function: cenit_context_free
 *  Releases all the memory allocated by the <cenit_context_new> function
 *  but also if present, this function releases the memory of the <CenitAst>
 *  object.
 */
void cenit_context_free(CenitContext *ctx)
{
    if (!ctx)
        return;

    cenit_symtable_free(&ctx->symtable);

    if (ctx->srcinfo)
        cenit_source_free(ctx->srcinfo);

    if (ctx->ast)
        cenit_ast_free(ctx->ast);

    if (ctx->errors)
        fl_array_free_each(ctx->errors, error_free);
}

/*
 * Function: cenit_context_error
 *  Initializes the *errors* property if needed and allocates memory for the formatted string. Both objects are
 *  deallocated by the <error_free> function.
 */
void cenit_context_error(CenitContext *ctx, CenitSourceLocation location, CenitErrorType type, const char *message, ...)
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
        .location = location,
        .message = formatted_msg
    };

    ctx->errors = fl_array_append(ctx->errors, &error);
}
