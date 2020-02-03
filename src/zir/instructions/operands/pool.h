#ifndef ZIR_OPERAND_POOL_H
#define ZIR_OPERAND_POOL_H


#include "array.h"
#include "reference.h"
#include "symbol.h"
#include "uint.h"

/*
 * Struct: struct ZirOperandPool
 *  An object that keeps track of the creation of operand objects
 * 
 * Members:
 *  <FlList> *operands: The list of created operands
 */
struct ZirOperandPool {
    FlList *operands;
};

/*
 * Function: zir_operand_pool_new
 *  Creates a new pool of operands
 *
 * Parameters:
 *  <void>: This function does not take parameters
 *
 * Returns:
 *  <struct ZirOperandPool>*: The operands pool object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_operand_pool_free> function
 */
struct ZirOperandPool* zir_operand_pool_new(void);

/*
 * Function: zir_operand_pool_free
 *  Frees the memory of the pool and all the objects within it
 *
 * Parameters:
 *  <struct ZirOperandPool> *pool: The pool object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_operand_pool_free(struct ZirOperandPool *pool);

/*
 * Function: zir_operand_pool_new_array
 *  Creates a new array operand and adds it to the pool
 *
 * Parameters:
 *  <struct ZirOperandPool> *pool: The pool object
 *  <struct ZirArrayType> *type: The type of the array's members
 *
 * Returns:
 *  struct ZirArrayOperand*: The array operand
 *
 * Notes:
 *   The pool object takes ownership of the <struct ZirArrayOperand> object, which means it will release
 *  the array operand memory when the <zir_operand_pool_free> function is called with the pool object 
 *  as argument.
 */
struct ZirArrayOperand* zir_operand_pool_new_array(struct ZirOperandPool *pool, struct ZirArrayType *type);

/*
 * Function: zir_operand_pool_new_reference
 *  Creates a new reference operand and adds it to the pool
 *
 * Parameters:
 *  <struct ZirOperandPool> *pool: The pool object
 *  <struct ZirReferenceType> *type: The type of the reference object
 *  <struct ZirSymbolOperand> *operand: The operand of the referenced object
 *
 * Returns:
 *  struct ZirReferenceOperand*: The reference operand
 *
 * Notes:
 *   The pool object takes ownership of the <struct ZirReferenceOperand> object, which means it will release
 *  the reference operand memory when the <zir_operand_pool_free> function is called with the pool object 
 *  as argument.
 */
struct ZirReferenceOperand* zir_operand_pool_new_reference(struct ZirOperandPool *pool, struct ZirReferenceType *type, struct ZirSymbolOperand *operand);

/*
 * Function: zir_operand_pool_new_symbol
 *  Creates a new symbol operand and adds it to the pool
 *
 * Parameters:
 *  <struct ZirOperandPool> *pool: The pool object
 *  <struct ZirSymbol> *symbol: The symbol object
 *
 * Returns:
 *  struct ZirSymbolOperand*: The symbol operand
 *
 * Notes:
 *   The pool object takes ownership of the <struct ZirSymbolOperand> object, which means it will release
 *  the symbol operand memory when the <zir_operand_pool_free> function is called with the pool object 
 *  as argument.
 */
struct ZirSymbolOperand* zir_operand_pool_new_symbol(struct ZirOperandPool *pool, struct ZirSymbol *symbol);

/*
 * Function: zir_operand_pool_new_uint
 *  Creates a new uint operand and adds it to the pool
 *
 * Parameters:
 *  <struct ZirOperandPool> *pool: The pool object
 *  <struct ZirUintType> *type: The type and size of the uint object
 *  <struct ZirUintValue> value: The value of the uint object
 *
 * Returns:
 *  struct ZirUintOperand*: The uint operand
 *
 * Notes:
 *   The pool object takes ownership of the <struct ZirUintOperand> object, which means it will release
 *  the uint operand memory when the <zir_operand_pool_free> function is called with the pool object 
 *  as argument.
 */
struct ZirUintOperand* zir_operand_pool_new_uint(struct ZirOperandPool *pool, struct ZirUintType *type, union ZirUintValue value);

#endif /* ZIR_OPERAND_POOL_H */
