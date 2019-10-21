#ifndef CENIT_CONTEXT_H
#define CENIT_CONTEXT_H

#include "ast.h"
#include "symtable.h"
#include "source.h"

/*
 * Macro: cenit_context_error_count
 *  Returns the size of the *errors* property of a <CenitContext> object
 *
 * Parameters:
 *  ctxptr - A pointer to a <CenitContext> object
 */
#define cenit_context_error_count(ctxptr) ((size_t)((ctxptr)->errors ? fl_array_length((ctxptr)->errors) : 0))


/*
 * Macro: cenit_context_has_errors
 *  Evaluates to *true* if the *errors* property in the <CenitContext> object is
 *  greater than 0
 *
 * Parameters:
 *  ctxptr - A pointer to a <CenitContext> object
 */
#define cenit_context_has_errors(ctxptr) (cenit_context_error_count((ctxptr)) > 0)

/*
 * Enum: CenitErrorType 
 *  Represents every type of error that might occur in the
 *  compilation process
 *
 */
typedef enum CenitErrorType {
    // Cenit specific errors
    CENIT_ERROR_INTERNAL,

    // Mostly missing/unexpected tokens
    CENIT_ERROR_SYNTAX,

    // Number cannot be represented by any integer type
    CENIT_ERROR_LARGE_INTEGER,

    // Symbol couldn't be found
    CENIT_ERROR_MISSING_SYMBOL,

    // Symbol already exists
    CENIT_ERROR_DUPLICATED_SYMBOL,

    // Cannot infer the type of an expression
    CENIT_ERROR_INFERENCE,

    // An operation involves types that aren't compatible
    CENIT_ERROR_TYPE_MISSMATCH,
} CenitErrorType;

/*
 * Struct: CenitError
 *  All the errors that occur in the compilation are tracked by
 *  objects of this type
 * 
 * Members:
 *  message: The error explanation
 *  location: Where in the source the error occurred tracked by a <CenitSourceLocation> object
 *  type: The type of the error being one of the <CenitErrorType> values
 */
typedef struct CenitError {
    const char *message;
    CenitSourceLocation location;
    CenitErrorType type;
} CenitError;

/*
 * Struct: CenitContext
 *  Keeps track of the information generated in the compilation
 *  process.
 * 
 * Members:
 *  errors - Array of <CenitError> objects that occur in the compilation process
 *  ast - After the parse process this property contains the <CenitAst> object
 *  srcinfo - <CenitSourceInfo> object
 *  symtable - Populated with the symbol information that is collected in the compilation
 */
typedef struct CenitContext {
    CenitError *errors;
    CenitAst *ast;
    CenitSourceInfo *srcinfo;
    CenitSymbolTable symtable;
} CenitContext;

/*
 * Function: cenit_context_new
 *  Creates a new <CenitContext> object that can be used to start the compilation process.
 *
 * Parameters:
 *  id - A string that represents the context's symbol table (<CenitSymbolTable>) identifier
 *  type - The origin or source from where to get the program's source code (CenitSourceType)
 *  input - Based on the *type* parameter this could be a filename or the program's source code
 *
 * Returns:
 *  CenitContext - An object to ready to be used in the compilation process
 *
 * Notes:
 *  The object returned by this function internally allocates memory for its properties, so
 *  the user must free that memory with the <cenit_context_free> function
 */
CenitContext cenit_context_new(const char *id, CenitSourceType type, const char *input);

/*
 * Function: cenit_context_free
 *  Releases the memory of an object allocated with the <cenit_context_new> function
 *
 * Parameters:
 *  ctx - <CenitContext> object to free
 *
 * Returns:
 *  void - This function does not return a value
 */
void cenit_context_free(CenitContext* ctx);

/*
 * Function: cenit_context_error
 *  Adds a new <CenitError> object to the <CenitContext>'s *errors* property
 *
 * Parameters:
 *  ctx - <CenitContext> object to free
 *  location - <CenitSourceLocation> object that contains information of where in the source code the error happened
 *  type - The <CenitErrorType> that represents the type of the error
 *  message - A string that accepts format specifiers containing information about the error
 *  ... - Depending on the format string, the function may expect a sequence of additional arguments, each containing 
 *          a value to be used to replace a format specifier in the format string.
 *
 * Returns:
 *  void - This function does not return a value
 */
void cenit_context_error(CenitContext *ctx, CenitSourceLocation location, CenitErrorType type, const char *message, ...);


#endif /* CENIT_CONTEXT_H */
