#ifndef ZIR_OPERAND_POOL_H
#define ZIR_OPERAND_POOL_H


#include "array.h"
#include "bool.h"
#include "reference.h"
#include "struct.h"
#include "symbol.h"
#include "uint.h"

/*
 * Struct: ZirOperandPool
 *  An object that keeps track of the creation of operand objects
 * 
 * Members:
 *  <FlList> *operands: The list of created operands
 */
typedef struct ZirOperandPool {
    FlList *operands;
} ZirOperandPool;

/*
 * Function: zir_operand_pool_new
 *  Creates a new pool of operands
 *
 * Parameters:
 *  <void>: This function does not take parameters
 *
 * Returns:
 *  <ZirOperandPool>*: The operands pool object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_operand_pool_free> function
 */
ZirOperandPool* zir_operand_pool_new(void);

/*
 * Function: zir_operand_pool_free
 *  Frees the memory of the pool and all the objects within it
 *
 * Parameters:
 *  <ZirOperandPool> *pool: The pool object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_operand_pool_free(ZirOperandPool *pool);

/*
 * Function: zir_operand_pool_new_array
 *  Creates a new array operand and adds it to the pool
 *
 * Parameters:
 *  <ZirOperandPool> *pool: The pool object
 *  <ZirArrayType> *type: The type of the array's members
 *
 * Returns:
 *  ZirArrayOperand*: The array operand
 *
 * Notes:
 *  The pool object takes ownership of the <ZirArrayOperand> object, which means it will release
 *  the array operand memory when the <zir_operand_pool_free> function is called with the pool object 
 *  as argument.
 */
ZirArrayOperand* zir_operand_pool_new_array(ZirOperandPool *pool, ZirArrayType *type);

/*
 * Function: zir_operand_pool_new_struct
 *  Creates a new struct operand and adds it to the pool
 *
 * Parameters:
 *  <ZirOperandPool> *pool: The pool object
 *  <ZirArrayType> *type: The type of the struct operand
 *
 * Returns:
 *  ZirStructOperand*: The struct operand
 *
 * Notes:
 *  The pool object takes ownership of the <ZirArrayOperand> object, which means it will release
 *  the struct operand memory when the <zir_operand_pool_free> function is called with the pool object 
 *  as argument.
 */
ZirStructOperand* zir_operand_pool_new_struct(ZirOperandPool *pool, ZirStructType *type);

/*
 * Function: zir_operand_pool_new_reference
 *  Creates a new reference operand and adds it to the pool
 *
 * Parameters:
 *  <ZirOperandPool> *pool: The pool object
 *  <ZirReferenceType> *type: The type of the reference object
 *  <ZirSymbolOperand> *operand: The operand of the referenced object
 *
 * Returns:
 *  ZirReferenceOperand*: The reference operand
 *
 * Notes:
 *  The pool object takes ownership of the <ZirReferenceOperand> object, which means it will release
 *  the reference operand memory when the <zir_operand_pool_free> function is called with the pool object 
 *  as argument.
 */
ZirReferenceOperand* zir_operand_pool_new_reference(ZirOperandPool *pool, ZirReferenceType *type, ZirSymbolOperand *operand);

/*
 * Function: zir_operand_pool_new_symbol
 *  Creates a new symbol operand and adds it to the pool
 *
 * Parameters:
 *  <ZirOperandPool> *pool: The pool object
 *  <ZirSymbol> *symbol: The symbol object
 *
 * Returns:
 *  ZirSymbolOperand*: The symbol operand
 *
 * Notes:
 *  The pool object takes ownership of the <ZirSymbolOperand> object, which means it will release
 *  the symbol operand memory when the <zir_operand_pool_free> function is called with the pool object 
 *  as argument.
 */
ZirSymbolOperand* zir_operand_pool_new_symbol(ZirOperandPool *pool, ZirSymbol *symbol);

/*
 * Function: zir_operand_pool_new_uint
 *  Creates a new uint operand and adds it to the pool
 *
 * Parameters:
 *  <ZirOperandPool> *pool: The pool object
 *  <ZirUintType> *type: The type and size of the uint object
 *  <struct ZirUintValue> value: The value of the uint object
 *
 * Returns:
 *  ZirUintOperand*: The uint operand
 *
 * Notes:
 *  The pool object takes ownership of the <ZirUintOperand> object, which means it will release
 *  the uint operand memory when the <zir_operand_pool_free> function is called with the pool object 
 *  as argument.
 */
ZirUintOperand* zir_operand_pool_new_uint(ZirOperandPool *pool, ZirUintType *type, ZirUintValue value);

/*
 * Function: zir_operand_pool_new_bool
 *  Creates a new boolean operand and adds it to the pool
 *
 * Parameters:
 *  <ZirOperandPool> *pool: The pool object
 *  <ZirBoolType> *type: The type of the boolean object
 *  <bool> value: The value of the boolean object
 *
 * Returns:
 *  ZirBoolOperand*: The boolean operand
 *
 * Notes:
 *  The pool object takes ownership of the <ZirBoolOperand> object, which means it will release
 *  the boolean operand memory when the <zir_operand_pool_free> function is called with the pool object 
 *  as argument.
 */
ZirBoolOperand* zir_operand_pool_new_bool(ZirOperandPool *pool, ZirBoolType *type, bool value);

#endif /* ZIR_OPERAND_POOL_H */
