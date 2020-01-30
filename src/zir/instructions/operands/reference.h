#ifndef ZIR_OPERAND_REFERENCE_H
#define ZIR_OPERAND_REFERENCE_H

#include "operand.h"
#include "symbol.h"

/*
 * Struct: struct ZirReferenceOperand
 *  A reference operand contains information about its type and a pointer to a symbol
 *   operand which is the referenced expression
 * 
 * Members:
 *  <struct ZirOperand> base: Basic operand information
 *  <struct ZirReferenceType> *type: The type information of the reference
 *  <struct ZirSymbolOperand> *operand: Pointer to the referenced symbol's operand
 */
struct ZirReferenceOperand {
    struct ZirOperand base;
    struct ZirReferenceType *type;
    struct ZirSymbolOperand *operand;
};

/*
 * Function: zir_operand_reference_new
 *  Creates a new reference operand object that references the symbol pointed by the symbol operand. The
 *  *type* object is the reference's type
 *
 * Parameters:
 *  <struct ZirReferenceType> *type: The type of the reference
 *  <struct ZirSymbolOperand> *operand: The referenced symbol operand
 *
 * Returns:
 *  struct ZirReferenceOperand*: The new reference operand object
 *
 * Notes:
 *  The object returned by this function must be freed using the <zir_operand_reference_free> function.
 *  The reference operand takes ownership of the <struct ZirReferenceType> object, which means it will release
 *  the type object memory too when <zir_operand_reference_free> is called with the reference operand as argument.
 */
struct ZirReferenceOperand* zir_operand_reference_new(struct ZirReferenceType *type, struct ZirSymbolOperand *operand);

/*
 * Function: zir_operand_reference_free
 *  Frees the memory of the reference operand
 *
 * Parameters:
 *  <struct ZirReferenceOperand> *reference_operand: Reference operand object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_operand_reference_free(struct ZirReferenceOperand *reference_operand);

/*
 * Function: zir_operand_reference_dump
 *  Dumps the string representation of the reference operand to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_operand_reference_dump(reference_operand, output);
 * ===========
 *
 * Parameters:
 *  <struct ZirReferenceOperand> *reference_operand: Reference operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_reference_dump(struct ZirReferenceOperand *reference_operand, char *output);

/*
 * Function: zir_operand_reference_type_dump
 *  Dumps the string representation of the type of the reference operand to the *output* 
 *  pointer. Because the *output* pointer can be modified this function returns 
 *  the same pointer, so it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_operand_reference_type_dump(reference_operand, output);
 * ===========
 *
 * Parameters:
 *  <struct ZirReferenceOperand> *reference_operand: Operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_reference_type_dump(struct ZirReferenceOperand *reference_operand, char *output);

#endif /* ZIR_OPERAND_REFERENCE_H */
