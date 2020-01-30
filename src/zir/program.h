#ifndef ZIR_PROGRAM_H
#define ZIR_PROGRAM_H

#include "block.h"

/*
 * Struct: struct ZirProgram
 *  Represents a ZIR program
 * 
 * Members:
 *  <struct ZirBlock> *global: A pointer to the global block or block
 *  <struct ZirBlock> *current: A pointer to the current block
 *  <struct ZirOperandPool> *operands: Keeps track of the operands. (Work as a root aggregate for operand objects)
 */
struct ZirProgram {
    struct ZirBlock *global;
    struct ZirBlock *current;
    struct ZirOperandPool *operands;
};

/*
 * Function: zir_program_new
 *  Creates a new ZIR program object
 *
 * Returns:
 *  <struct ZirProgram>* - The created program
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zir_program_free> function
 */
struct ZirProgram* zir_program_new(void);

/*
 * Function: zir_program_free
 *  Releases the memory of the program object
 *
 * Parameters:
 *  <struct ZirProgram> *program - Program object to be freed
 *
 * Returns:
 *  void - This function does not return a value
 */
void zir_program_free(struct ZirProgram *program);

/*
 * Function: zir_program_enter_block
 *  Sets the provided block as the current block in the program as long as the current block is the
 *  parent of the provided block
 *
 * Parameters:
 *  <struct ZirProgram> *program: Program object
 *  <struct ZirBlock> *block: Target block 
 *
 * Returns:
 *  bool: *true* if the provided block can be selected as the current block, otherwise *false*.
 */
bool zir_program_enter_block(struct ZirProgram *program, struct ZirBlock *block);

/*
 * Function: zir_program_push_block
 *  This function sets a block of the provided type and with the provided name as the current block within the 
 *  program. If the block does not exist, this function creates it. In case the block already exists, this function
 *  uses that block as the target block to be selected as "current".
 *
 * Parameters:
 *  <struct ZirProgram> *program: Program object
 *  <enum ZirBlockType> typ: Type of the target block
 *  <const char> *name: Name of the target block
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_program_push_block(struct ZirProgram *program, enum ZirBlockType type, const char *name);

/*
 * Function: zir_program_pop_block
 *  Changes the current block by selecting the current block's parent as the new current block
 *
 * Parameters:
 *  <struct ZirProgram> *program: Program object
 *
 * Returns:
 *  void: This function does not return a value
 */
void zir_program_pop_block(struct ZirProgram *program);

/*
 * Function: zir_program_has_block
 *  Returns *true* or *false* based on if a block with the provided type and name exists in the program and
 *  can be reached from the current block.
 *
 * Parameters:
 *  <struct ZirProgram> *program: Program object
 *  <enum ZirBlockType> type: Block type
 *  <const char> *name: Block name
 *
 * Returns:
 *  bool: *true* if a block with that type and name exists, otherwise *false*.
 */
bool zir_program_has_block(struct ZirProgram *program, enum ZirBlockType type, const char *name);

/*
 * Function: zir_program_get_block
 *  Returns -if it exists- a block of the provided type and provided name that is reachable from the current block
 *
 * Parameters:
 *  <struct ZirProgram> *program: Program object
 *  <enum ZirBlockType> type: Type of the target block
 *  <const char> *name: Name of the target block
 *
 * Returns:
 *  struct ZirBlock*: Pointer to the target block if it exists, otherwise this function returns <NULL>
 */
struct ZirBlock* zir_program_get_block(struct ZirProgram *program, enum ZirBlockType type, const char *name);

/*
 * Function: zir_program_add_symbol
 *  Adds a new symbol to the current block
 *
 * Parameters:
 *  <struct ZirProgram> *program - Program object
 *  <struct ZirSymbol> *symbol - Symbol to be added
 * 
 * Returns:
 *  <struct ZirSymbol>* - Added symbol
 * 
 */
struct ZirSymbol* zir_program_add_symbol(struct ZirProgram *program, struct ZirSymbol *symbol);

/*
 * Function: zir_program_emit
 *  Adds a new instruction to the current program's block
 *
 * Parameters:
 *  <struct ZirProgram> *program - Program object
 *  <struct ZirInstruction> *instruction - Instruction to be added
 * 
 * Returns:
 *  <struct ZirInstruction>* - Added instruction
 * 
 */
struct ZirInstruction* zir_program_emit(struct ZirProgram *program, struct ZirInstruction *instruction);

/*
 * Function: zir_program_dump
 *  Returns a heap allocated string containing a dump of the program object
 *
 * Parameters:
 *  <struct ZenitProgram> *program: Program object to dump to the output
 *
 * Returns:
 *  char*: Pointer to a heap allocated string containing the dump of the program
 *
 * Notes:
 *  The string returned by this function must be freed with the <fl_cstring_free> function
 */
char* zir_program_dump(struct ZirProgram *program);

#endif /* ZIR_PROGRAM_H */
