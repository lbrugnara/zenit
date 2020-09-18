#include <stdint.h>
#include <inttypes.h>
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
#include "instructions/if-false.h"
#include "instructions/jump.h"

#include "../nes.h"
#include "../../../zir/symtable.h"
#include "../../../zir/symbol.h"
#include "../../../zir/instructions/operands/operand.h"

typedef void(*ZirInstructionVisitor)(ZnesContext *znes_context, ZirInstr *instruction, ZirBlock *block);
static void visit_zir_instruction(ZnesContext *znes_context, ZirInstr *instruction, ZirBlock *block);
static void visit_zir_variable_instruction(ZnesContext *znes_context, ZirVariableInstr *instruction, ZirBlock *block);
static void visit_cast_instruction(ZnesContext *znes_context, ZirCastInstr *instruction, ZirBlock *block);
static void visit_if_false_instruction(ZnesContext *znes_context, ZirIfFalseInstr *instruction, ZirBlock *block);
static void visit_jump_instruction(ZnesContext *znes_context, ZirJumpInstr *instruction, ZirBlock *block);

static const ZirInstructionVisitor zir_instruction_visitors[] = {
    [ZIR_INSTR_VARIABLE]    = (ZirInstructionVisitor) &visit_zir_variable_instruction,
    [ZIR_INSTR_CAST]        = (ZirInstructionVisitor) &visit_cast_instruction,
    [ZIR_INSTR_IF_FALSE]    = (ZirInstructionVisitor) &visit_if_false_instruction,
    [ZIR_INSTR_JUMP]        = (ZirInstructionVisitor) &visit_jump_instruction,
};

static bool znes_allocation_setup_aggregates(ZnesContext *znes_context, ZnesAlloc *allocation, ZirType *zir_type)
{
    if (allocation->type == ZNES_ALLOC_TYPE_BOOL
        || allocation->type == ZNES_ALLOC_TYPE_UINT
        || allocation->type == ZNES_ALLOC_TYPE_REFERENCE
        || allocation->type == ZNES_ALLOC_TYPE_TEMP)
        return true;

    if (zir_type->typekind == ZIR_TYPE_ARRAY)
    {
        ZirArrayType *zir_array_type = (ZirArrayType*) zir_type;

        // This is an array allocation
        ZnesArrayAlloc *znes_array_allocation = (ZnesArrayAlloc*) allocation;

        // The allocation type for each member of the array
        ZnesAllocType znes_array_allocation_type = znes_alloc_type_from_zir_type(zir_array_type->member_type);

        // The size of each member of the array
        size_t znes_item_alloc_size = zir_type_size(zir_array_type->member_type, ZNES_POINTER_SIZE);

        for (size_t i = 0; i < zir_array_type->length; i++)
        {
            // The name of the members goes from "0" to "zir_array_type->length - 1"
            char item_name[1024] = { 0 };
            size_t to_write = snprintf(NULL, 0, "%zu", i);
            snprintf(item_name, 1024, "%zu", i);            

            // Create the item allocation
            ZnesAlloc *znes_item_allocation = znes_alloc_new(znes_array_allocation_type,            // Allocation type is the type of the members
                                                item_name,                                          // The item name being the position within the array
                                                znes_array_allocation->base.segment,                // The segment is the same one of the array
                                                znes_item_alloc_size,                               // The size of the allocation is defined by the members type
                                                allocation->address + (znes_item_alloc_size * i));  // The address is based on the position of the item and its size

            // The item might be an aggregate too, we ensure we setup all the allocations recursively
            znes_allocation_setup_aggregates(znes_context, znes_item_allocation, zir_array_type->member_type);

            // Finally we setup the allocation within the array
            znes_array_alloc_add_element(znes_array_allocation, znes_item_allocation);
        }

        return true;
    }
    else if (zir_type->typekind == ZIR_TYPE_STRUCT)
    {
        ZirStructType *zir_struct_type = (ZirStructType*) zir_type;

        // This is a struct allocation
        ZnesStructAlloc *znes_struct_allocation = (ZnesStructAlloc*) allocation;

        // The gap between members is defined by the size of each member
        size_t gap = 0;

        // We iterate through the struct members
        struct FlListNode *node = fl_list_head(zir_struct_type->members);
        while (node)
        {
            ZirStructTypeMember *zir_struct_member = (ZirStructTypeMember*) node->value;

            // The allocation type for each member of the struct
            ZnesAllocType znes_member_allocation_type = znes_alloc_type_from_zir_type(zir_struct_member->type);

            // The size of each member of the struct
            size_t znes_member_alloc_size = zir_type_size(zir_struct_member->type, ZNES_POINTER_SIZE);

            // Create the member allocation
            ZnesAlloc *znes_member_allocation = znes_alloc_new(znes_member_allocation_type,             // Allocation type is defined by the member type
                                                                zir_struct_member->name,                // The name is the name of the struct member
                                                                znes_struct_allocation->base.segment,   // The segment is the same one of the struct
                                                                znes_member_alloc_size,                 // The size of the allocation is defined by the members type
                                                                allocation->address + gap);             // The address is based on the current gap

            // The member might be an aggregate too, we ensure we setup all the allocations recursively
            znes_allocation_setup_aggregates(znes_context, znes_member_allocation, zir_struct_member->type);

            // Finally we setup the allocation within the struct
            znes_struct_alloc_add_member(znes_struct_allocation, znes_member_allocation);

            // We update the gap for the next member's address
            gap += znes_member_alloc_size;

            node = node->next;
        }

        return true;
    }

    znes_context_error(znes_context, ZNES_ERROR_INTERNAL, "Unhandled ZIR type in znes_allocation_setup_aggregates");

    return false;
}

static void visit_jump_instruction(ZnesContext *znes_context, ZirJumpInstr *zir_instruction, ZirBlock *zir_block)
{
    // The destination of an jump instruction is a jump offset
    // TODO: WE ARE USING UINT FOR THE JUMP, WE SHOULD UPDATE THIS TO SIGNED INT AS THE TYPES IN ZIR CHANGE
    //       TO ALLOW JUMPING BACKWARDS
    if (zir_instruction->base.destination->type != ZIR_OPERAND_UINT)
    {
        znes_context_error(znes_context, ZNES_ERROR_INTERNAL, "Invalid operand type for jump offset in visit_jump_instruction");
        return;
    }

    ZnesUintOperand *jump_offset = (ZnesUintOperand*) znes_utils_make_nes_operand(znes_context, zir_instruction->base.destination);

    if (jump_offset == NULL)
        return;

    ZnesJumpInstruction *jump_instruction = znes_jump_instruction_new(jump_offset);

    znes_program_emit_instruction(znes_context->program, (ZnesInstruction*) jump_instruction);
}

static void visit_if_false_instruction(ZnesContext *znes_context, ZirIfFalseInstr *zir_instruction, ZirBlock *zir_block)
{
    // The destination of an if_false instruction is a jump offset
    // TODO: WE ARE USING UINT FOR THE JUMP, WE SHOULD UPDATE THIS TO SIGNED INT AS THE TYPES IN ZIR CHANGE
    //       TO ALLOW JUMPING BACKWARDS
    if (zir_instruction->base.destination->type != ZIR_OPERAND_UINT)
    {
        znes_context_error(znes_context, ZNES_ERROR_INTERNAL, "Invalid operand type for jump offset in visit_if_false_instruction");
        return;
    }
    ZnesUintOperand *jump_offset = (ZnesUintOperand*) znes_utils_make_nes_operand(znes_context, zir_instruction->base.destination);

    if (jump_offset == NULL)
        return;

    // Create the source operand
    ZnesOperand *znes_source_operand = znes_utils_make_nes_operand(znes_context, zir_instruction->source);

    if (znes_source_operand == NULL)
        return;

    ZnesIfFalseInstruction *if_false_instruction = znes_if_false_instruction_new(jump_offset, znes_source_operand);

    znes_program_emit_instruction(znes_context->program, (ZnesInstruction*) if_false_instruction);
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
static void visit_cast_instruction(ZnesContext *znes_context, ZirCastInstr *instruction, ZirBlock *block)
{
    // A cast has a source value, we need to create a NES operand
    ZnesOperand *znes_source_operand = znes_utils_make_nes_operand(znes_context, instruction->source);

    if (znes_source_operand == NULL)
        return;

    // A cast creates has a destination symbol
    ZirSymbol *zir_destination_symbol = ((ZirSymbolOperand*) instruction->base.destination)->symbol;

    // We request allocation for the temporal destination symbol in the NES program
    ZnesAllocRequest znes_alloc_request = { 0 };
    if (!znes_alloc_request_init(znes_context, block, zir_destination_symbol, NULL, &znes_alloc_request))
        return;

    // With the allocation request and the initial value we can allocate the space for the temporal variable
    ZnesAlloc *znes_allocation = znes_program_alloc_variable(znes_context->program, zir_destination_symbol->name, &znes_alloc_request, znes_source_operand);

    if (znes_allocation == NULL)
    {
        znes_context_error(znes_context, ZNES_ERROR_ALLOC, 
            "Could not satisfy allocation request in cast instruction for segment: %s | address: %"PRIX16" | size: %zu",
            znes_segment_kind_str[znes_alloc_request.segment],
            znes_alloc_request.address,
            znes_alloc_request.size);
        return;
    }

    // For arrays, structs, and other compund types, we need to configure its members
    if (!znes_allocation_setup_aggregates(znes_context, znes_allocation, zir_destination_symbol->type))
        return;
}

static void visit_zir_variable_instruction(ZnesContext *znes_context, ZirVariableInstr *zir_instruction, ZirBlock *zir_block)
{
    // A variable is initialized from its source value, we need to create a NES operand
    ZnesOperand *znes_source_operand = znes_utils_make_nes_operand(znes_context, zir_instruction->source);
    
    if (znes_source_operand == NULL) return;

    // A variable definition has a destination symbol
    ZirSymbol *zir_destination_symbol = ((ZirSymbolOperand*) zir_instruction->base.destination)->symbol;

    // We request allocation for the destination symbol in the NES program
    ZnesAllocRequest znes_alloc_request = { 0 };
    if (!znes_alloc_request_init(znes_context, zir_block, zir_destination_symbol, zir_instruction->attributes, &znes_alloc_request))
        return;

    // With the allocation request and the initial value we can allocate the space for the variable
    ZnesAlloc *znes_allocation = znes_program_alloc_variable(znes_context->program, zir_destination_symbol->name, &znes_alloc_request, znes_source_operand);

    if (znes_allocation == NULL)
    {
        znes_context_error(znes_context, ZNES_ERROR_ALLOC, 
            "Could not satisfy allocation request in variable instruction for segment: %s | address: %"PRIX16" | size: %zu",
            znes_segment_kind_str[znes_alloc_request.segment],
            znes_alloc_request.address,
            znes_alloc_request.size);
        return;
    }

    // For arrays, structs, and other compund types, we need to configure its members
    if (!znes_allocation_setup_aggregates(znes_context, znes_allocation, zir_destination_symbol->type))
        return;
}

static void visit_zir_instruction(ZnesContext *znes_context, ZirInstr *zir_instruction, ZirBlock *zir_block)
{
    zir_instruction_visitors[zir_instruction->type](znes_context, zir_instruction, zir_block);
}

static void visit_zir_block(ZnesContext *znes_context, ZirBlock *zir_block)
{
    size_t instr_count = fl_array_length(zir_block->instructions);

    for (size_t i=0; i < instr_count; i++)
        visit_zir_instruction(znes_context, zir_block->instructions[i], zir_block);
}

bool znes_generate_program(ZnesContext *znes_context, ZirProgram *zir_program)
{
    if (!zir_program)
        return NULL;

    // Start visiting the ZIR program
    visit_zir_block(znes_context, zir_program->global);

    if (znes_context_has_errors(znes_context))
    {
        znes_context_print_errors(znes_context);
    }

    return !znes_context_has_errors(znes_context);
}
