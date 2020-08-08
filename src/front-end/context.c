
#include "ast/ast.h"
#include "context.h"
#include "program.h"
#include "source.h"
#include "types/type.h"

/*
 * Function: error_free
 *  Frees the memory of an error object allocated with the <zenit_context_error> function
 *
 * Parameters:
 * <void> *errorptr: Pointer to a <struct ZenitError> object
 *
 * Returns:
 *  void: This function does not return a value
 *
 */
static void error_free(void *errorptr)
{
    if (!errorptr)
        return;

    struct ZenitError *error = (struct ZenitError*)errorptr;

    if (error->message)
        fl_cstring_free(error->message);

    fl_free(error);
}

static void error_add(FlByte **dest, const FlByte *src)
{
    *dest = fl_malloc(sizeof(struct ZenitError));
    memcpy(*dest, src, sizeof(struct ZenitError));
}

/*
 * Function: zenit_context_new
 *  Allocates memory for a <struct ZenitSymtable> object and a <struct ZenitSourceInfo> object
 */
ZenitContext zenit_context_new(enum ZenitSourceType type, const char *input)
{
    ZenitContext ctx = { 
        .program = zenit_program_new(),
        .srcinfo = zenit_source_new(type, input),
        .types = zenit_type_ctx_new(),
        .errors = NULL
    };

    return ctx;
}

/*
 * Function: zenit_context_free
 *  Releases all the memory allocated by the <zenit_context_new> function, the errors
 *  array, and also if present, this function releases the memory of the <struct ZenitAst>
 *  object.
 */
void zenit_context_free(ZenitContext *ctx)
{
    if (!ctx)
        return;

    zenit_program_free(ctx->program);

    if (ctx->srcinfo)
        zenit_source_free(ctx->srcinfo);

    if (ctx->ast)
        zenit_ast_free(ctx->ast);

    if (ctx->errors)
        fl_list_free(ctx->errors);

    if (ctx->types)
        zenit_type_ctx_free(ctx->types);
}

/*
 * Function: zenit_context_error
 *  Initializes the *errors* array if needed and appends a new error object to it. The memory allocated for the *errors*
 *  array and the memory allocated for each error object is freed in the <zenit_context_free> function
 */
void zenit_context_error(ZenitContext *ctx, struct ZenitSourceLocation location, ZenitErrorType type, const char *message, ...)
{
    if (message == NULL)
        return;

    if (ctx->errors == NULL)
    {
        ctx->errors = fl_list_new_args((struct FlListArgs) {
            .value_allocator = (FlContainerAllocatorFn) error_add,
            .value_cleaner = (FlContainerCleanupFn) error_free
        });
    }

    va_list args;
    va_start(args, message);
    char *formatted_msg = fl_cstring_vadup(message, args);
    va_end(args);

    struct ZenitError error = {
        .type = type,
        .location = location,
        .message = formatted_msg
    };

    struct FlListNode *tmp = fl_list_head(ctx->errors);

    while (tmp != NULL)
    {
        struct ZenitError *err = (struct ZenitError*) tmp->value;

        if (error.location.filename == err->location.filename 
            && (error.location.line < err->location.line || (error.location.line == err->location.line && error.location.col < err->location.col)))
        {
            fl_list_insert_before(ctx->errors, tmp, &error);
            return;
        }

        tmp = tmp->next;
    }

    fl_list_append(ctx->errors, &error);
}

void zenit_context_print_errors(ZenitContext *ctx)
{
    if (!zenit_context_has_errors(ctx))
        return;

    struct FlListNode *tmp = fl_list_head(ctx->errors);

    while (tmp != NULL)
    {
        struct ZenitError *error = (struct ZenitError*) tmp->value;

        fprintf(stderr, "%s:%d:%d: %s\n", 
            error->location.filename, 
            error->location.line, 
            error->location.col, 
            error->message
        );

        tmp = tmp->next;
    }
}
