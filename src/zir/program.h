#ifndef ZIR_PROGRAM_H
#define ZIR_PROGRAM_H

#include "block.h"
#include "types/pool.h"

/*
 * Struct: struct ZirProgram
 *  Represents a ZIR program
 * 
 * Members:
 *  <struct ZirBlock> *global: A pointer to the global block or scope
 *  <struct ZirBlock> *current: A pointer to the current scope
 */
struct ZirProgram {
    struct ZirBlock *global;
    struct ZirBlock *current;
    struct ZirTypePool type_pool;
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
 * 
 */
void zir_program_free(struct ZirProgram *program);

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
 *  Dumps the string representation of the program to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zir_program_dump(program, output);
 * ===========
 *
 * Parameters:
 *  program - Program object
 *  output - Output buffer
 *
 * Returns:
 *  char* - *output* pointer
 *
 */
char* zir_program_dump(struct ZirProgram *program, char *output);

#endif /* ZIR_PROGRAM_H */
