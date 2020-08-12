#include <stdint.h>

#include "generate.h"

#include "utils.h"
#include "operands/array.h"
#include "operands/bool.h"
#include "operands/reference.h"
#include "operands/struct.h"
#include "operands/uint.h"
#include "operands/variable.h"
#include "objects/array.h"
#include "objects/struct.h"

#include "../../../zir/symtable.h"
#include "../../../zir/symbol.h"
#include "../../../zir/instructions/operands/operand.h"

typedef void(*ZirInstructionVisitor)(ZirInstr *instruction, ZirBlock *block, ZnesProgram *program);
static void visit_instruction(ZirInstr *instruction, ZirBlock *block, ZnesProgram *program);
static void visit_variable_instruction(ZirVariableInstr *instruction, ZirBlock *block, ZnesProgram *program);
static void visit_cast_instruction(ZirCastInstr *instruction, ZirBlock *block, ZnesProgram *program);

static const ZirInstructionVisitor instruction_visitors[] = {
    [ZIR_INSTR_VARIABLE]   = (ZirInstructionVisitor) &visit_variable_instruction,
    [ZIR_INSTR_CAST]       = (ZirInstructionVisitor) &visit_cast_instruction,
};

static bool allocate_aggregates(ZnesProgram *program, ZnesAlloc *variable, ZirType *zir_type)
{
    if (variable->type == ZNES_ALLOC_TEMP)
        return true;

    if (zir_type->typekind == ZIR_TYPE_ARRAY)
    {
        ZnesArrayAlloc *znes_array_variable = (ZnesArrayAlloc*) variable;

        ZirArrayType *zir_array_type = (ZirArrayType*) zir_type;
        ZnesAllocType znes_items_type = znes_utils_variable_type_from_zir_type(zir_array_type->member_type);
        size_t item_size = zir_type_size(zir_array_type->member_type, 2 /* bytes */);

        for (size_t i = 0; i < zir_array_type->length; i++)
        {
            char item_name[1024] = { 0 };
            size_t to_write = snprintf(NULL, 0, "%zu", i);
            snprintf(item_name, 1024, "%zu", i);            

            ZnesAlloc *element = znes_alloc_new(znes_items_type, 
                                                        item_name, 
                                                        variable->segment, 
                                                        item_size, 
                                                        variable->address + (item_size * i));

            allocate_aggregates(program, element, zir_array_type->member_type);

            znes_array_alloc_add_element(znes_array_variable, element);
        }
    }
    else if (zir_type->typekind == ZIR_TYPE_STRUCT)
    {
        ZnesStructAlloc *znes_struct_variable = (ZnesStructAlloc*) variable;

        ZirStructType *zir_struct_type = (ZirStructType*) zir_type;

        size_t gap = 0;
        struct FlListNode *node = fl_list_head(zir_struct_type->members);

        while (node)
        {
            ZirStructTypeMember *zir_struct_member = (ZirStructTypeMember*) node->value;
            
            ZnesAllocType znes_member_var_type = znes_utils_variable_type_from_zir_type(zir_struct_member->type);
            size_t member_size = zir_type_size(zir_struct_member->type, 2 /* bytes */);

            ZnesAlloc *member_variable = znes_alloc_new(znes_member_var_type, 
                                                                zir_struct_member->name, 
                                                                variable->segment, 
                                                                member_size, 
                                                                variable->address + gap);

            allocate_aggregates(program, member_variable, zir_struct_member->type);

            znes_struct_alloc_add_member(znes_struct_variable, member_variable);

            gap += member_size;

            node = node->next;
        }
    }

    return false;
}

/*
 * Function: visit_cast_instruction
 *  The destination of a cast instruction is always a temporal variable, so we need to store the casted expression (the source operand)
 *  into the destination operand (the temporal variable)
 *
 * Parameters:
 *  <ZirCastInstr> *instruction: ZIR instruction
 *  <ZirBlock> *block: The current block where the ZIR instruction comes from
 *  <CompatZnesProgram> *program: The NES program being built
 *
 * Returns:
 *  void: This function does not return a value
 * 
 */
static void visit_cast_instruction(ZirCastInstr *instruction, ZirBlock *block, ZnesProgram *program)
{
    ZirSymbol *zir_symbol = ((ZirSymbolOperand*) instruction->base.destination)->symbol;

    ZnesAllocRequest var_alloc = { 0 };
    if (!znes_utils_var_alloc_prepare(program, block, zir_symbol, NULL, &var_alloc))
    {
        // TODO: Handle errors
        return;
    }

    ZnesOperand *source = znes_utils_make_nes_operand(program, instruction->source);
    
    if (source == NULL)
    {
        // TODO: Handle errors
        return;
    }

    ZnesAlloc *variable = znes_program_alloc_variable(program, zir_symbol->name, &var_alloc, source);

    if (variable == NULL)
    {
        // TODO: Handle errors
        return;
    }

    if (!allocate_aggregates(program, variable, zir_symbol->type))
    {
        // TODO: Handle errors
        return;
    }
}

/*
 * Function: visit_variable_instruction
 *  A variable declaration is a store from the source operand to the destination operand, which in this case
 *  is the place where the variable is being stored.
 *
 * Parameters:
 *  <ZirVariableInstr> *instruction: ZIR instruction
 *  <ZirBlock> *block: The current block where the ZIR instruction comes from
 *  <CompatZnesProgram> *program: The NES program being built
 *
 * Returns:
 *  void: This function does not return a value
 * 
 */
static void visit_variable_instruction(ZirVariableInstr *instruction, ZirBlock *block, ZnesProgram *program)
{
    ZirSymbol *zir_symbol = ((ZirSymbolOperand*) instruction->base.destination)->symbol;

    ZnesAllocRequest var_alloc = { 0 };
    if (!znes_utils_var_alloc_prepare(program, block, zir_symbol, instruction->attributes, &var_alloc))
    {
        // TODO: Handle errors
        return;
    }

    ZnesOperand *source = znes_utils_make_nes_operand(program, instruction->source);
    
    if (source == NULL)
    {
        // TODO: Handle errors
        return;
    }

    ZnesAlloc *variable = znes_program_alloc_variable(program, zir_symbol->name, &var_alloc, source);

    if (variable == NULL)
    {
        // TODO: Handle errors
        return;
    }

    if (!allocate_aggregates(program, variable, zir_symbol->type))
    {
        // TODO: Handle errors
        return;
    }
}

static void visit_instruction(ZirInstr *instruction, ZirBlock *block, ZnesProgram *program)
{
    instruction_visitors[instruction->type](instruction, block, program);
}

static void visit_block(ZirBlock *block, ZnesProgram *program)
{
    size_t instr_count = fl_array_length(block->instructions);

    for (size_t i=0; i < instr_count; i++)
    {
        ZirInstr *instr = block->instructions[i];
        visit_instruction(instr, block, program);
    }
}

ZnesProgram* znes_generate_program(ZirProgram *zir_program)
{
    if (!zir_program)
        return NULL;

    // Create the program object
    ZnesProgram *program = znes_program_new();

    // Start visiting the ZIR program
    ZirBlock *zir_block = zir_program->current;
    visit_block(zir_block, program);

    return program;
}
