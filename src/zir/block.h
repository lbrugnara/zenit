#ifndef ZENIT_IR_BLOCK_H
#define ZENIT_IR_BLOCK_H

#include "symtable.h"
#include "instructions/instruction.h"
#include "instructions/cast.h"
#include "instructions/variable.h"

/*
 * Struct: zenit_ir_block_new
 *  A ZIR block represents a scope in the program that contains a symbol table and a set
 *  of instructions
 * 
 * Members:
 *  <struct ZenitIrBlock> *parent: Pointer to the parent block
 *  <struct ZenitIrBlock> **children: Set of children blocks
 *  <struct ZenitIrInstruction> **instructions: Set of block instructions
 *  <struct ZenitIrSymbolTable> symtable: Symbol table of the current block
 * 
 */
struct ZenitIrBlock {
    struct ZenitIrBlock *parent;
    struct ZenitIrBlock **children;
    struct ZenitIrInstruction **instructions;
    struct ZenitIrSymbolTable symtable;
    unsigned long long temp_counter;
};

/*
 * Function: zenit_ir_block_new
 *  Creates a new block object
 *
 * Parameters:
 *  <>  - 
 *
 * Returns:
 *  <struct ZenitIrBlock>* - The created block object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_ir_block_free> function
 */
struct ZenitIrBlock* zenit_ir_block_new(const char *id, enum ZenitIrSymbolTableType type, struct ZenitIrBlock *parent);

/*
 * Function: zenit_ir_block_free
 *  Releases the memory of the block object
 *
 * Parameters:
 *  <struct ZenitIrBlock> *block - Block to be freed
 *
 * Returns:
 *  void - This function does not return a value
 * 
 */
void zenit_ir_block_free(struct ZenitIrBlock *block);

/*
 * Function: zenit_ir_block_dump
 *  Dumps the string representation of the block to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zenit_ir_block_dump(block, output);
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
char* zenit_ir_block_dump(struct ZenitIrBlock *block, char *output);

#endif /* ZENIT_IR_BLOCK_H */
