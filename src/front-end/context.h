#ifndef ZENIT_CONTEXT_H
#define ZENIT_CONTEXT_H

#include "ast/ast.h"
#include "symtable.h"
#include "source.h"

/*
 * Macro: zenit_context_error_count
 *  Returns the size of the *errors* property of a <struct ZenitContext> object
 *
 * Parameters:
 *  ctxptr - A pointer to a <struct ZenitContext> object
 */
#define zenit_context_error_count(ctxptr) ((size_t)((ctxptr)->errors ? fl_array_length((ctxptr)->errors) : 0))


/*
 * Macro: zenit_context_has_errors
 *  Evaluates to *true* if the *errors* property in the <struct ZenitContext> object is
 *  greater than 0
 *
 * Parameters:
 *  ctxptr - A pointer to a <struct ZenitContext> object
 */
#define zenit_context_has_errors(ctxptr) (zenit_context_error_count((ctxptr)) > 0)

/*
 * Enum: enum ZenitErrorType 
 *  Represents every type of error that might occur in the
 *  compilation process
 *
 */
enum ZenitErrorType {
    // Zenit specific errors
    ZENIT_ERROR_INTERNAL,

    // Mostly missing/unexpected tokens
    ZENIT_ERROR_SYNTAX,

    // Number cannot be represented by any integer type
    ZENIT_ERROR_LARGE_INTEGER,

    // Symbol couldn't be found
    ZENIT_ERROR_MISSING_SYMBOL,

    // Symbol already exists
    ZENIT_ERROR_DUPLICATED_SYMBOL,

    // Cannot infer the type of an expression
    ZENIT_ERROR_INFERENCE,

    // An operation involves types that are incompatible
    ZENIT_ERROR_TYPE_MISSMATCH,

    // Error taking a reference from an expression
    ZENIT_ERROR_INVALID_REFERENCE,
};

/*
 * Struct: struct ZenitError
 *  All the errors that occur in the compilation are tracked by
 *  objects of this type
 * 
 * Members:
 *  message: The error explanation
 *  location: Where in the source the error occurred tracked by a <struct ZenitSourceLocation> object
 *  type: The type of the error being one of the <enum ZenitErrorType> values
 */
struct ZenitError {
    const char *message;
    struct ZenitSourceLocation location;
    enum ZenitErrorType type;
};

/*
 * Struct: struct ZenitContext
 *  Keeps track of the information generated in the compilation
 *  process.
 * 
 * Members:
 *  errors - Array of <struct ZenitError> objects that occur in the compilation process
 *  ast - After the parse process this property contains the <struct ZenitAst> object
 *  srcinfo - <struct ZenitSourceInfo> object
 *  program - The object that contains the program being compiled
 */
struct ZenitContext {
    struct ZenitError *errors;
    struct ZenitAst *ast;
    struct ZenitSourceInfo *srcinfo;
    struct ZenitProgram *program;
};

/*
 * Function: zenit_context_new
 *  Creates a new <struct ZenitContext> object that can be used to start the compilation process.
 *
 * Parameters:
 *  type - The origin or source from where to get the program's source code (enum ZenitSourceType)
 *  input - Based on the *type* parameter this could be a filename or the program's source code
 *
 * Returns:
 *  struct ZenitContext - An object to ready to be used in the compilation process
 *
 * Notes:
 *  The object returned by this function internally allocates memory for its properties, so
 *  the user must free that memory with the <zenit_context_free> function
 */
struct ZenitContext zenit_context_new(enum ZenitSourceType type, const char *input);

/*
 * Function: zenit_context_free
 *  Releases the memory of an object allocated with the <zenit_context_new> function
 *
 * Parameters:
 *  ctx - <struct ZenitContext> object to free
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_context_free(struct ZenitContext* ctx);

/*
 * Function: zenit_context_error
 *  Adds a new <struct ZenitError> object to the <struct ZenitContext>'s *errors* property
 *
 * Parameters:
 *  ctx - <struct ZenitContext> object to free
 *  location - <struct ZenitSourceLocation> object that contains information of where in the source code the error happened
 *  type - The <enum ZenitErrorType> that represents the type of the error
 *  message - A string that accepts format specifiers containing information about the error
 *  ... - Depending on the format string, the function may expect a sequence of additional arguments, each containing 
 *          a value to be used to replace a format specifier in the format string.
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_context_error(struct ZenitContext *ctx, struct ZenitSourceLocation location, enum ZenitErrorType type, const char *message, ...);

void zenit_context_print_errors(struct ZenitContext *ctx);

#endif /* ZENIT_CONTEXT_H */
