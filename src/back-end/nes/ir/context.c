#include <fllib/Mem.h>
#include "context.h"
#include "program.h"

/*
 * Function: error_free
 *  Frees the memory of an error object allocated with the <znes_context_error> function
 *
 * Parameters:
 * <void> *errorptr: Pointer to a <ZnesError> object
 *
 * Returns:
 *  void: This function does not return a value
 *
 */
static void error_free(void *errorptr)
{
    if (!errorptr)
        return;

    ZnesError *error = (ZnesError*) errorptr;

    if (error->message)
        fl_cstring_free(error->message);

    fl_free(error);
}

static void error_add(FlByte **dest, const FlByte *src)
{
    *dest = fl_malloc(sizeof(ZnesError));
    memcpy(*dest, src, sizeof(ZnesError));
}

/*
 * Function: znes_context_new
 *  Allocates memory for a <struct ZnesSymtable> object and a <ZnesSourceInfo> object
 */
ZnesContext* znes_context_new(bool scripting)
{
    ZnesContext *ctx = fl_malloc(sizeof(ZnesContext));
    ctx->program = znes_program_new(scripting);
    ctx->operands = znes_operand_pool_new();
    ctx->errors = NULL;
    return ctx;
}

/*
 * Function: znes_context_free
 *  Releases all the memory allocated by the <znes_context_new> function, the errors
 *  array, and also if present, this function releases the memory of the <ZnesAst>
 *  object.
 */
void znes_context_free(ZnesContext *ctx)
{
    if (!ctx) return;

    if (ctx->program) znes_program_free(ctx->program);
    if (ctx->errors) fl_list_free(ctx->errors);
    if (ctx->operands) znes_operand_pool_free(ctx->operands);
    fl_free(ctx);
}

/*
 * Function: znes_context_error
 *  Initializes the *errors* array if needed and appends a new error object to it. The memory allocated for the *errors*
 *  array and the memory allocated for each error object is freed in the <znes_context_free> function
 */
void znes_context_error(ZnesContext *ctx, ZnesErrorType type, const char *message, ...)
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

    ZnesError error = {
        .type = type,
        .message = formatted_msg
    };

    fl_list_append(ctx->errors, &error);
}

void znes_context_print_errors(ZnesContext *ctx)
{
    if (!znes_context_has_errors(ctx))
        return;

    struct FlListNode *tmp = fl_list_head(ctx->errors);

    while (tmp != NULL)
    {
        ZnesError *error = (ZnesError*) tmp->value;

        fprintf(stderr, "%s\n", error->message);

        tmp = tmp->next;
    }
}
