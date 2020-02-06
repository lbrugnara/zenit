#include <stdint.h>

#include "generate.h"
#include "program.h"
#include "emit.h"

#include "../../zir/symtable.h"
#include "../../zir/instructions/operands/operand.h"
#include "../../zir/instructions/operands/array.h"
#include "../../zir/instructions/operands/uint.h"
#include "../../zir/instructions/operands/reference.h"
#include "../../zir/instructions/operands/symbol.h"

typedef void(*ZirBlockVisitor)(struct ZirBlock *block, struct ZenitNesProgram *program);
static void visit_block(struct ZirBlock *block, struct ZenitNesProgram *program);

typedef void(*ZirInstructionVisitor)(struct ZirInstruction *instruction, struct ZirBlock *block, struct ZenitNesProgram *program);
static void visit_instruction(struct ZirInstruction *instruction, struct ZirBlock *block, struct ZenitNesProgram *program);
static void visit_variable_instruction(struct ZirVariableInstruction *instruction, struct ZirBlock *block, struct ZenitNesProgram *program);
static void visit_cast_instruction(struct ZirCastInstruction *instruction, struct ZirBlock *block, struct ZenitNesProgram *program);

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
 *  <struct ZirCastInstruction> *instruction: ZIR instruction
 *  <struct ZirBlock> *block: The current block where the ZIR instruction comes from
 *  <struct ZenitNesProgram> *program: The NES program being built
 *
 * Returns:
 *  void: This function does not return a value
 * 
 */
static void visit_cast_instruction(struct ZirCastInstruction *instruction, struct ZirBlock *block, struct ZenitNesProgram *program)
{
    struct ZirSymbol *zir_symbol = ((struct ZirSymbolOperand*) instruction->base.destination)->symbol;
    struct ZenitNesSymbol *nes_symbol = zenit_nes_program_reserve_symbol(program, block, NULL, zir_symbol);
    
    if (instruction->source->type == ZIR_OPERAND_UINT)
    {
        zenit_nes_emit_store_uint(program, (struct ZirUintOperand*) instruction->source, nes_symbol, 0);
    }
    else if (instruction->source->type == ZIR_OPERAND_ARRAY)
    {
        zenit_nes_emit_store_array(program, (struct ZirArrayOperand*) instruction->source, nes_symbol, 0, nes_symbol->element_size);
    }
    else if (instruction->source->type == ZIR_OPERAND_STRUCT)
    {
        zenit_nes_emit_store_struct(program, (struct ZirStructOperand*) instruction->source, nes_symbol, 0);
    }
    else if (instruction->source->type == ZIR_OPERAND_REFERENCE)
    {
        zenit_nes_emit_store_reference(program, (struct ZirReferenceOperand*) instruction->source, nes_symbol, 0);
    }
    else if (instruction->source->type == ZIR_OPERAND_SYMBOL)
    {
        zenit_nes_emit_store_symbol(program, (struct ZirSymbolOperand*) instruction->source, nes_symbol, 0);
    }
}

/*
 * Function: visit_variable_instruction
 *  A variable declaration is a store from the source operand to the destination operand, which in this case
 *  is the place where the variable is being stored.
 *
 * Parameters:
 *  <struct ZirVariableInstruction> *instruction: ZIR instruction
 *  <struct ZirBlock> *block: The current block where the ZIR instruction comes from
 *  <struct ZenitNesProgram> *program: The NES program being built
 *
 * Returns:
 *  void: This function does not return a value
 * 
 */
static void visit_variable_instruction(struct ZirVariableInstruction *instruction, struct ZirBlock *block, struct ZenitNesProgram *program)
{
    struct ZirSymbol *zir_symbol = ((struct ZirSymbolOperand*) instruction->base.destination)->symbol;
    struct ZenitNesSymbol *nes_symbol = zenit_nes_program_reserve_symbol(program, block, instruction->attributes, zir_symbol);

    if (instruction->source->type == ZIR_OPERAND_UINT)
    {
        zenit_nes_emit_store_uint(program, (struct ZirUintOperand*) instruction->source, nes_symbol, 0);
    }
    else if (instruction->source->type == ZIR_OPERAND_ARRAY)
    {
        zenit_nes_emit_store_array(program, (struct ZirArrayOperand*) instruction->source, nes_symbol, 0, nes_symbol->element_size);
    }
    else if (instruction->source->type == ZIR_OPERAND_STRUCT)
    {
        zenit_nes_emit_store_struct(program, (struct ZirStructOperand*) instruction->source, nes_symbol, 0);
    }
    else if (instruction->source->type == ZIR_OPERAND_REFERENCE)
    {
        zenit_nes_emit_store_reference(program, (struct ZirReferenceOperand*) instruction->source, nes_symbol, 0);
    }
    else if (instruction->source->type == ZIR_OPERAND_SYMBOL)
    {
        zenit_nes_emit_store_symbol(program, (struct ZirSymbolOperand*) instruction->source, nes_symbol, 0);
    }
}

static void visit_instruction(struct ZirInstruction *instruction, struct ZirBlock *block, struct ZenitNesProgram *program)
{
    instruction_visitors[instruction->type](instruction, block, program);
}

static void visit_block(struct ZirBlock *block, struct ZenitNesProgram *program)
{
    size_t instr_count = fl_array_length(block->instructions);

    for (size_t i=0; i < instr_count; i++)
    {
        struct ZirInstruction *instr = block->instructions[i];
        visit_instruction(instr, block, program);
    }
}

struct ZenitNesProgram* zenit_nes_generate_program(struct ZirProgram *zir_program)
{
    if (!zir_program)
        return NULL;

    // Create the program object
    struct ZenitNesProgram *nes_program = zenit_nes_program_new();

    // Start visiting the ZIR program
    struct ZirBlock *zir_block = zir_program->current;
    visit_block(zir_block, nes_program);

    return nes_program;
}
