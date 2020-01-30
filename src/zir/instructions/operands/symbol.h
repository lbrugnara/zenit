#ifndef ZIR_OPERAND_SYMBOL_H
#define ZIR_OPERAND_SYMBOL_H

#include "operand.h"
#include "../../symbol.h"

/*
 * Struct: struct ZirSymbolOperand
 *  A symbol operand contains a reference to a symbol object
 * 
 * Members:
 *  <struct ZirOperand> base: Basic operand information
 *  <struct ZirSymbol> *symbol: The symbol object
 */
struct ZirSymbolOperand {
    struct ZirOperand base;
    struct ZirSymbol *symbol;
};

/*
 * Function: zir_operand_symbol_new
 *  Creates a new symbol operand object
 *
 * Parameters:
 *  <struct ZirSymbol> *symbol: The symbol object
 *
 * Returns:
 *  struct ZirSymbolOperand*: The new symbol operand object
 *
 * Notes:
 *  The object returned by this function must be freed using the <zir_operand_symbol_free> function.
 */
struct ZirSymbolOperand* zir_operand_symbol_new(struct ZirSymbol *symbol);

/*
 * Function: zir_operand_symbol_free
 *  Frees the memory of the symbol operand
 *
 * Parameters:
 *  <struct ZirSymbolOperand> *symbol_operand: Symbol operand object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_operand_symbol_free(struct ZirSymbolOperand *symbol_operand);

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
 *  <struct ZirSymbolOperand> *symbol_operand: Symbol operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_symbol_dump(struct ZirSymbolOperand *symbol_operand, char *output);

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
 *  <struct ZirSymbolOperand> *symbol_operand: Operand object
 *  <char> *output: Output buffer
 *
 * Returns:
 *  <char>*: *output* pointer
 *
 * Notes:
 *  If the reallocation of the *output* pointer fails, this function frees its memory.
 */
char* zir_operand_symbol_type_dump(struct ZirSymbolOperand *symbol_operand, char *output);

#endif /* ZIR_OPERAND_SYMBOL_H */
