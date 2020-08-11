#ifndef ZIR_BLOCK_H
#define ZIR_BLOCK_H

#include "symtable.h"
#include "instructions/instruction.h"
#include "instructions/cast.h"
#include "instructions/if-false.h"
#include "instructions/jump.h"
#include "instructions/variable.h"

/*
 * Enum: ZirBlockType
 *  Represents the different type of symbol tables
 * 
 */
typedef enum ZirBlockType {
    ZIR_BLOCK_GLOBAL,
    ZIR_BLOCK_STRUCT,
    ZIR_BLOCK_FUNCTION,
} ZirBlockType;

/*
 * Struct: zir_block_new
 *  A ZIR block represents a scope in the program that contains a symbol table and a set
 *  of instructions
 * 
 * Members:
 *  <ZirBlock> *parent: Pointer to the parent block
 *  <ZirBlock> **children: Set of children blocks
 *  <ZirInstr> **instructions: Set of block instructions
 *  <ZirSymtable> symtable: Symbol table of the current block
 * 
 */
typedef struct ZirBlock {
    const char *id;
    struct ZirBlock *parent;
    struct ZirBlock **children;
    ZirInstr **instructions;
    ZirSymtable symtable;
    unsigned long long temp_counter;
    ZirBlockType type;
} ZirBlock;

/*
 * Function: zir_block_new
 *  Creates a new block object
 *
 * Parameters:
 *  <>  - 
 *
 * Returns:
 *  <ZirBlock>* - The created block object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_block_free> function
 */
ZirBlock* zir_block_new(const char *id, ZirBlockType type, ZirBlock *parent);

/*
 * Function: zir_block_free
 *  Releases the memory of the block object
 *
 * Parameters:
 *  <ZirBlock> *block - Block to be freed
 *
 * Returns:
 *  void - This function does not return a value
 * 
 */
void zir_block_free(ZirBlock *block);

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
char* zir_block_dump(ZirBlock *block, char *output);

/*
 * Function: zir_block_get_ip
 *  Returns the block's instruction pointer, which is the pointer to the last instruction
 *  within the block
 *
 * Parameters:
 *  block - The block object
 *
 * Returns:
 *  size_t - Instruction pointer value
 */
size_t zir_block_get_ip(ZirBlock *block);

/*
 * Function: zir_block_has_symbol
 *  Returns true if a symbol with the provided name exists in the block
 *
 * Parameters:
 *  block - The block object
 *  name - The symbol name to be found
 *
 * Returns:
 *  bool - *true* if the symbol exists, otherwise it returns *false*.
 */
bool zir_block_has_symbol(ZirBlock *block, const char *name);

#endif /* ZIR_BLOCK_H */
