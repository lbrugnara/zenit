#include "generate.h"
#include "emit-alloc.h"
#include "emit-if-false.h"
#include "emit-jump.h"

static bool emit_instruction(Rp2a03Program *program, Rp2a03TextSegment *segment, bool is_startup, ZnesInstruction *instruction)
{
    switch (instruction->kind)
    {
        case ZNES_INSTRUCTION_ALLOC:
            return rp2a03_emit_alloc_instruction(program, segment, is_startup, (ZnesAllocInstruction*) instruction);

        case ZNES_INSTRUCTION_IF_FALSE:
            return rp2a03_emit_if_false_instruction(program, segment, is_startup, (ZnesIfFalseInstruction*) instruction);

        case ZNES_INSTRUCTION_JUMP:
            return rp2a03_emit_jump_instruction(program, segment, is_startup, (ZnesJumpInstruction*) instruction);

        default: break;
    }

    return false;
}

static void emit_segment_instructions(Rp2a03Program *rp2a03_program, Rp2a03TextSegment *rp2a03_segment, ZnesProgram *ir_prog, ZnesTextSegment *ir_segment)
{
    ZnesInstructionListNode *inst_node = znes_instruction_list_head(ir_segment->instructions);

    while (inst_node)
    {
        ZnesInstruction *instr = (ZnesInstruction*) inst_node->value;
        
        if (!emit_instruction(rp2a03_program, rp2a03_segment, ir_prog->startup_context, instr))
        {
            // TODO: Error handling
            break;
        }

        rp2a03_text_segment_backpatch_jumps(rp2a03_segment);

        inst_node = inst_node->next;
    }
}

Rp2a03Program* rp2a03_generate_program(ZnesProgram *ir_prog)
{
    Rp2a03Program *program = rp2a03_program_new(ir_prog->data->base_address, ir_prog->startup->base_address, ir_prog->code->base_address);

    // DATA segment is allocated using the startup routine:
    //  a) if a symbol within the DATA segment is initialized with a constant value, the value is copied on compilation
    //  b) if the value is not constant (reading from ZP, or CODE) the startup routine emits an instruction to initialize it
    ir_prog->startup_context = true;
    emit_segment_instructions(program, program->startup, ir_prog, ir_prog->startup);
    ir_prog->startup_context = false;
    emit_segment_instructions(program, program->code, ir_prog, ir_prog->code);

    // 0x00 is used as an special sentinel. Address $00 is ZP, it never can be a valid base address, we need to 
    // find the place for the startup routine
    if (program->startup->base_address == 0x0)
    {
        // PC points to the last byte + 1, but also let space for the JMP instruction to jump to
        // the user defined reset interrupt handler (the startup routine will be the ACTUAL reset handler)
        size_t needed_space = program->startup->pc + 3;

        // We start looking from the DATA segment address
        size_t i = 0, j = 0;
        for (; i < fl_array_length(program->data->slots); i++)
        {
            if (program->data->slots[i] != 0)
            {
                j = i + 1;
                continue;
            }

            if (i - j >= needed_space)
                break;
        }

        if (i - j < needed_space)
        {
            // TODO: Error handling
            rp2a03_program_free(program);
            return NULL;
        }        

        program->startup->base_address = program->data->base_address + j;

        // Flag the DATA segment as used on the startup range
        /*for (; j < i; j++)
            program->data->slots[j] = 1;*/

        rp2a03_text_segment_backpatch_absolute_jumps(program->startup);
    }

    // 0x00 is used as an special sentinel. Address $00 is ZP, it never can be a valid base address, we need to 
    // find the place for the CODE segment
    if (program->code->base_address == 0x0)
    {
        // We start looking from the DATA segment address
        size_t needed_space = program->code->pc; // PC points to the last byte + 1

        size_t i = 0, j = 0;
        for (; i < fl_array_length(program->data->slots); i++)
        {
            if (program->data->slots[i] != 0)
            {
                j = i + 1;
                continue;
            }

            if (i - j >= needed_space)
                break;
        }

        if (i - j < needed_space)
        {
            // TODO: Error handling
            rp2a03_program_free(program);
            return NULL;
        }

        program->code->base_address = program->data->base_address + j;

        // Flag the DATA segment as used on the startup range
        /*for (; j < i; j++)
            program->data->slots[j] = 1;*/

        rp2a03_text_segment_backpatch_absolute_jumps(program->code);
    }

    return program;
}
