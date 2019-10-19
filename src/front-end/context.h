#ifndef CENIT_CONTEXT_H
#define CENIT_CONTEXT_H

#include "ast.h"
#include "symtable.h"

#define cenit_context_error_count(ctxptr) ((size_t)((ctxptr)->errors ? fl_array_length((ctxptr)->errors) : 0))

typedef enum CenitErrorType {
    // Cenit specific errors
    CENIT_ERROR_INTERNAL,

    // Mostly missing/unexpected tokens
    CENIT_ERROR_SYNTAX,

    // Number cannot be represented by Cenit
    CENIT_ERROR_LARGE_INTEGER,

    CENIT_ERROR_DUPLICATED_SYMBOL
} CenitErrorType;

/*
 * Type: CenitError
 *  Represents an error in the parser
 */
typedef struct CenitError {
    const char *message;
    CenitErrorType type;
    unsigned int line;
    unsigned int col;
} CenitError;


typedef struct CenitContext {
    CenitError *errors;
    CenitAst *ast;
    CenitSymbolTable symtable;
} CenitContext;

CenitContext cenit_context_new(const char *id);
void cenit_context_free(CenitContext*);
void cenit_context_error(CenitContext *ctx, CenitErrorType type, unsigned int line, unsigned int col, const char *message, ...);


#endif /* CENIT_CONTEXT_H */
