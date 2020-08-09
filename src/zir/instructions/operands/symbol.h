#ifndef ZIR_OPERAND_SYMBOL_H
#define ZIR_OPERAND_SYMBOL_H

#include "operand.h"
#include "../../symbol.h"

/*
 * Struct: ZirSymbolOperand
 *  A symbol operand contains a reference to a symbol object
 * 
 * Members:
 *  <ZirOperand> base: Basic operand information
 *  <ZirSymbol> *symbol: The symbol object
 */
typedef struct ZirSymbolOperand {
    ZirOperand base;
    ZirSymbol *symbol;
} ZirSymbolOperand;

/*
 * Function: zir_operand_symbol_new
 *  Creates a new symbol operand object
 *
 * Parameters:
 *  <ZirSymbol> *symbol: The symbol object
 *
 * Returns:
 *  ZirSymbolOperand*: The new symbol operand object
 *
 * Notes:
 *  The object returned by this function must be freed using the <zir_operand_symbol_free> function.
 */
ZirSymbolOperand* zir_operand_symbol_new(ZirSymbol *symbol);

/*
 * Function: zir_operand_symbol_free
 *  Frees the memory of the symbol operand
 *
 * Parameters:
 *  <ZirSymbolOperand> *symbol_operand: Symbol operand object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_operand_symbol_free(ZirSymbolOperand *symbol_operand);

/*
 * Function: zir_operand_symbol_dump
 *  Dumps the string representation of the symbol operand to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_operand_symbol_dump(symbol_operand, output);
 * ===========
 *
 * Parameters:
 *  <ZirSymbolOperand> *symbol_operand: Symbol operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_symbol_dump(ZirSymbolOperand *symbol_operand, char *output);

/*
 * Function: zir_operand_symbol_type_dump
 *  Dumps the string representation of the type of the symbol operand to the *output* 
 *  pointer. Because the *output* pointer can be modified this function returns 
 *  the same pointer, so it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_operand_symbol_type_dump(symbol_operand, output);
 * ===========
 *
 * Parameters:
 *  <ZirSymbolOperand> *symbol_operand: Operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_symbol_type_dump(ZirSymbolOperand *symbol_operand, char *output);

#endif /* ZIR_OPERAND_SYMBOL_H */
