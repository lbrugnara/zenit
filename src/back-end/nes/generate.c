#include <stdint.h>

#include "generate.h"
#include "program.h"

#include "emitters/emit.h"

#include "../../zir/symtable.h"
#include "../../zir/instructions/operands/operand.h"

typedef void(*ZirBlockVisitor)(ZirBlock *block, ZnesProgram *program);
static void visit_block(ZirBlock *block, ZnesProgram *program);

typedef void(*ZirInstructionVisitor)(ZirInstruction *instruction, ZirBlock *block, ZnesProgram *program);
static void visit_instruction(ZirInstruction *instruction, ZirBlock *block, ZnesProgram *program);
static void visit_variable_instruction(ZirVariableInstruction *instruction, ZirBlock *block, ZnesProgram *program);
static void visit_cast_instruction(ZirCastInstruction *instruction, ZirBlock *block, ZnesProgram *program);

static const ZirInstructionVisitor instruction_visitors[] = {
    [ZIR_INSTR_VARIABLE]   = (ZirInstructionVisitor) &visit_variable_instruction,
    [ZIR_INSTR_CAST]       = (ZirInstructionVisitor) &visit_cast_instruction,
};

/*
 * Function: visit_cast_instruction
 *  The destination of a cast instruction is always a temporal variable, so we need to store the casted expression (the source operand)
 *  into the destination operand (the temporal variable)
 *
 * Parameters:
 *  <ZirCastInstruction> *instruction: ZIR instruction
 *  <ZirBlock> *block: The current block where the ZIR instruction comes from
 *  <ZnesProgram> *program: The NES program being built
 *
 * Returns:
 *  void: This function does not return a value
 * 
 */
static void visit_cast_instruction(ZirCastInstruction *instruction, ZirBlock *block, ZnesProgram *program)
{
    ZirSymbol *zir_symbol = ((ZirSymbolOperand*) instruction->base.destination)->symbol;
    ZnesSymbol *nes_symbol = znes_program_reserve_symbol(program, block, NULL, zir_symbol);
    
    znes_emitter_store(program, instruction->source, nes_symbol, 0);
}

/*
 * Function: visit_variable_instruction
 *  A variable declaration is a store from the source operand to the destination operand, which in this case
 *  is the place where the variable is being stored.
 *
 * Parameters:
 *  <ZirVariableInstruction> *instruction: ZIR instruction
 *  <ZirBlock> *block: The current block where the ZIR instruction comes from
 *  <ZnesProgram> *program: The NES program being built
 *
 * Returns:
 *  void: This function does not return a value
 * 
 */
static void visit_variable_instruction(ZirVariableInstruction *instruction, ZirBlock *block, ZnesProgram *program)
{
    ZirSymbol *zir_symbol = ((ZirSymbolOperand*) instruction->base.destination)->symbol;
    ZnesSymbol *nes_symbol = znes_program_reserve_symbol(program, block, instruction->attributes, zir_symbol);

    znes_emitter_store(program, instruction->source, nes_symbol, 0);
}

static void visit_instruction(ZirInstruction *instruction, ZirBlock *block, ZnesProgram *program)
{
    instruction_visitors[instruction->type](instruction, block, program);
}

static void visit_block(ZirBlock *block, ZnesProgram *program)
{
    size_t instr_count = fl_array_length(block->instructions);

    for (size_t i=0; i < instr_count; i++)
    {
        ZirInstruction *instr = block->instructions[i];
        visit_instruction(instr, block, program);
    }
}

ZnesProgram* znes_generate_program(ZirProgram *zir_program)
{
    if (!zir_program)
        return NULL;

    // Create the program object
    ZnesProgram *nes_program = znes_program_new();

    // Start visiting the ZIR program
    ZirBlock *zir_block = zir_program->current;
    visit_block(zir_block, nes_program);

    return nes_program;
}
