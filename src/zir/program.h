#ifndef ZENIT_IR_PROGRAM_H
#define ZENIT_IR_PROGRAM_H

#include "block.h"

/*
 * Struct: struct ZenitIrProgram
 *  Represents a ZIR program
 * 
 * Members:
 *  <struct ZenitIrBlock> *global: A pointer to the global block or scope
 *  <struct ZenitIrBlock> *current: A pointer to the current scope
 */
struct ZenitIrProgram {
    struct ZenitIrBlock *global;
    struct ZenitIrBlock *current;
};

/*
 * Function: zenit_ir_program_new
 *  Creates a new ZIR program object
 *
 * Returns:
 *  <struct ZenitIrProgram>* - The created program
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_ir_program_free> function
 */
struct ZenitIrProgram* zenit_ir_program_new(void);

/*
 * Function: zenit_ir_program_free
 *  Releases the memory of the program object
 *
 * Parameters:
 *  <struct ZenitIrProgram> *program - Program object to be freed
 *
 * Returns:
 *  void - This function does not return a value
 * 
 */
void zenit_ir_program_free(struct ZenitIrProgram *program);

/*
 * Function: zenit_ir_program_add_global_symbol
 *  Adds a new symbol to the global block
 *
 * Parameters:
 *  <struct ZenitIrProgram> *program - Program object
 *  <struct ZenitIrSymbol> *symbol - Symbol to be added
 * 
 * Returns:
 *  <struct ZenitIrSymbol>* - Added symbol
 * 
 */
struct ZenitIrSymbol* zenit_ir_program_add_global_symbol(struct ZenitIrProgram *program, struct ZenitIrSymbol *symbol);

/*
 * Function: zenit_ir_program_add_symbol
 *  Adds a new symbol to the current block
 *
 * Parameters:
 *  <struct ZenitIrProgram> *program - Program object
 *  <struct ZenitIrSymbol> *symbol - Symbol to be added
 * 
 * Returns:
 *  <struct ZenitIrSymbol>* - Added symbol
 * 
 */
struct ZenitIrSymbol* zenit_ir_program_add_symbol(struct ZenitIrProgram *program, struct ZenitIrSymbol *symbol);

/*
 * Function: zenit_ir_program_add_instruction
 *  Adds a new instruction to the current program's block
 *
 * Parameters:
 *  <struct ZenitIrProgram> *program - Program object
 *  <struct ZenitIrInstruction> *instruction - Instruction to be added
 * 
 * Returns:
 *  <struct ZenitIrInstruction>* - Added instruction
 * 
 */
struct ZenitIrInstruction* zenit_ir_program_add_instruction(struct ZenitIrProgram *program, struct ZenitIrInstruction *instruction);

/*
 * Function: zenit_ir_program_dump
 *  Dumps the string representation of the program to the *output* pointer. Because
 *  the *output* pointer can be modified this function returns the same pointer, so
 *  it is safe to use it as:
 * 
 * ==== C ====
 *  output = zenit_ir_program_dump(program, output);
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
char* zenit_ir_program_dump(struct ZenitIrProgram *program, char *output);

#endif /* ZENIT_IR_PROGRAM_H */
