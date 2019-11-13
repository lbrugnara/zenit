#ifndef ZIR_BLOCK_H
#define ZIR_BLOCK_H

#include "symtable.h"
#include "instruction.h"

/*
 * Struct: zir_block_new
 *  A ZIR block represents a scope in the program that contains a symbol table and a set
 *  of instructions
 * 
 * Members:
 *  <struct ZirBlock> *parent: Pointer to the parent block
 *  <struct ZirBlock> **children: Set of children blocks
 *  <struct ZirInstruction> **instructions: Set of block instructions
 *  <struct ZirSymbolTable> symtable: Symbol table of the current block
 * 
 */
struct ZirBlock {
    struct ZirBlock *parent;
    struct ZirBlock **children;
    struct ZirInstruction **instructions;
    struct ZirSymbolTable symtable;
};

/*
 * Function: zir_block_new
 *  Creates a new block object
 *
 * Parameters:
 *  <>  - 
 *
 * Returns:
 *  <struct ZirBlock>* - The created block object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_block_free> function
 */
struct ZirBlock* zir_block_new(const char *id, enum ZirSymbolTableType type, struct ZirBlock *parent);

/*
 * Function: zir_block_free
 *  Releases the memory of the block object
 *
 * Parameters:
 *  <struct ZirBlock> *block - Block to be freed
 *
 * Returns:
 *  void - This function does not return a value
 * 
 */
void zir_block_free(struct ZirBlock *block);

/*
 * Function: zir_block_dump
 *  Dumps the string representation of the block to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_block_dump(block, output);
 * ===========
 *
 * Parameters:
 *  block - Block object
 *  output - Output buffer
 *
 * Returns:
 *  char* - *output* pointer
 *
 */
char* zir_block_dump(struct ZirBlock *block, char *output);

#endif /* ZIR_BLOCK_H */
