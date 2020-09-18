#ifndef ZNES_CONTEXT_H
#define ZNES_CONTEXT_H

#include "program.h"
#include "operands/pool.h"

/*
 * Macro: znes_context_error_count
 *  Returns the number of *errors* registered in the <ZnesContext> object
 *
 * Parameters:
 *  <ZnesContext> *ctxptr: A pointer to a <ZnesContext> object
 */
#define znes_context_error_count(ctxptr) ((size_t)((ctxptr)->errors ? fl_list_length((ctxptr)->errors) : 0))


/*
 * Macro: znes_context_has_errors
 *  Evaluates to *true* if the are *errors* registered in the <ZnesContext> object 
 *
 * Parameters:
 *  <ZnesContext> *ctxptr: A pointer to a <ZnesContext> object
 */
#define znes_context_has_errors(ctxptr) ((ctxptr)->errors && fl_list_head((ctxptr)->errors) != NULL)

/*
 * Enum: ZnesErrorType 
 *  Represents every type of error that might occur in the
 *  compilation process
 *
 */
typedef enum ZnesErrorType {
    // Znes specific errors
    ZNES_ERROR_INTERNAL,
    // Error allocating objects in the program
    ZNES_ERROR_ALLOC,
} ZnesErrorType;

/*
 * Struct: ZnesError
 *  All the errors that occur in the compilation are tracked by objects of this type
 * 
 * Members:
 *  <const char> *message: The error explanation
 *  <ZnesErrorType> type: The type of the error being one of the <ZnesErrorType> values
 * 
 */
typedef struct ZnesError {
    const char *message;
    ZnesErrorType type;
} ZnesError;

typedef FlList ZnesErrorList;

/*
 * Struct: ZnesContext
 *  Keeps track of the information generated in the compilation
 *  process.
 * 
 * Members:
 *  <ZnesErrorList> *errors: List of <ZnesError> objects that occur in the compilation process
 *  <ZnesProgram> *program: The object that contains the program being compiled
 */
typedef struct ZnesContext {
    ZnesErrorList *errors;
    ZnesProgram *program;
    ZnesOperandPool *operands;
} ZnesContext;

/*
 * Function: znes_context_new
 *  Creates a new <ZnesContext> object that can be used to start the compilation process.
 *
 * Parameters:
 *  <bool> scripting: *true* if the program is in script mode
 *
 * Returns:
 *  <ZnesContext>: A context object ready to be used in the compilation process
 *
 * Notes:
 *  The object returned by this function internally allocates memory for its properties, so
 *  the user must free that memory with the <znes_context_free> function
 */
ZnesContext* znes_context_new(bool scripting);

/*
 * Function: znes_context_free
 *  Releases the memory of an object allocated with the <znes_context_new> function
 *
 * Parameters:
 *  <ZnesContext> *ctx: object to free
 *
 * Returns:
 *  void: This function does not return a value
 */
void znes_context_free(ZnesContext* ctx);

/*
 * Function: znes_context_error
 *  Adds a new <ZnesError> object to the <ZnesContext>'s *errors* property
 *
 * Parameters:
 *  <ZnesContext> *ctx: Context object
 *  <ZnesSourceLocation> location: Object that contains information of where in the source code the error happened
 *  <ZnesErrorType> type: The code that represents the type of the error
 *  <const char> *message: A string that accepts format specifiers containing information about the error
 *  *...*: Depending on the format string, the function may expect a sequence of additional arguments, each containing 
 *          a value to be used to replace a format specifier in the format string.
 *
 * Returns:
 *  void: This function does not return a value
 */
void znes_context_error(ZnesContext *ctx, ZnesErrorType type, const char *message, ...);

/*
 * Function: znes_context_print_errors
 *  Print all the errors registered in the context object to the standard error
 *
 * Parameters:
 *  <ZnesContext> *ctx - Context object
 *
 * Returns:
 *  void - This function does not return a value
 *
 */
void znes_context_print_errors(ZnesContext *ctx);

#endif /* ZNES_CONTEXT_H */
