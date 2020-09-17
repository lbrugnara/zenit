#include <fllib/Mem.h>
#include <fllib/Array.h>
#include <fllib/Cstring.h>
#include "segment-text.h"
#include "instruction.h"

void allocate_pending_jump(FlByte **dest, const FlByte *src)
{
    *dest = fl_malloc(sizeof(Rp2a03PendingJump));
    memcpy(*dest, src, sizeof(Rp2a03PendingJump));
}

Rp2a03TextSegment* rp2a03_text_segment_new(size_t base_address)
{
    Rp2a03TextSegment *text = fl_malloc(sizeof(Rp2a03TextSegment));

    text->pc = 0;
    text->bytes = fl_array_new(sizeof(uint8_t), UINT16_MAX);
    text->base_address = base_address;
    text->pending_jumps = fl_list_new_args((struct FlListArgs) { .value_allocator = allocate_pending_jump, .value_cleaner = fl_container_cleaner_pointer });

    return text;
}

void rp2a03_text_segment_free(Rp2a03TextSegment *text)
{
    fl_array_free(text->bytes);
    if (text->pending_jumps) fl_list_free(text->pending_jumps);
    fl_free(text);
}

void rp2a03_text_segment_add_pending_jump(Rp2a03TextSegment *text, Rp2a03PendingJump *pending_jump)
{
    fl_list_append(text->pending_jumps, pending_jump);
}

void rp2a03_text_segment_backpatch_jumps(Rp2a03TextSegment *text)
{
    Rp2a03PendingJumpListNode *node = fl_list_head(text->pending_jumps);

    Rp2a03PendingJumpListNode *remove = NULL;
    while (node)
    {
        Rp2a03PendingJump *pending_jump = (Rp2a03PendingJump*) node->value;

        if (pending_jump->ir_offset > 0)
            pending_jump->ir_offset--;

        if (pending_jump->ir_offset == 0)
        {
            if (pending_jump->absolute)
            {
                // Absolute
                // We don't have the base address yet, so we store the "base" offset
                pending_jump->base_jump_pc = text->pc;
            }
            else
            {
                // Relative
                uint8_t jump_offset = (uint8_t) ((text->pc - pending_jump->base_jump_pc) & 0xFF);
                text->bytes[pending_jump->byte_index] = jump_offset;
                // Relative nodes can be removed, they are not needed once they are backpatched
                remove = node;
            }

        }

        node = node->next;

        if (remove)
        {
            fl_list_remove(text->pending_jumps, remove);
            remove = NULL;
        }
    }
}

void rp2a03_text_segment_backpatch_absolute_jumps(Rp2a03TextSegment *text)
{
    Rp2a03PendingJumpListNode *node = fl_list_head(text->pending_jumps);

    while (node)
    {
        Rp2a03PendingJump *pending_jump = (Rp2a03PendingJump*) node->value;

        // TODO: Handle error if pending_jump->ir_offset is not zero
        // TODO: Handle error if pending_jump->absolute is false

        if (pending_jump->absolute)
        {
            // Update the base address
            uint16_t jump_dest = pending_jump->base_jump_pc + text->base_address;
            text->bytes[pending_jump->byte_index] = (uint8_t) (jump_dest & 0xFF);
            text->bytes[pending_jump->byte_index + 1] = (uint8_t) ((jump_dest >> 8) & 0xFF);
        }

        Rp2a03PendingJumpListNode *remove = node;
        node = node->next;
        fl_list_remove(text->pending_jumps, remove);
    }
}

char* rp2a03_text_segment_disassemble(Rp2a03TextSegment *text, char *title, char *output)
{
    // Startup
    // TODO: Move this to the segment-text module
    if (text->pc == 0)
        return output;

    fl_cstring_vappend(&output, "; %s\n\n", title);
    for (size_t pc = 0; pc < text->pc;)
    {
        Rp2a03Instruction *instr = rp2a03_instruction_lookup(text->bytes[pc]);

        fl_cstring_vappend(&output, "%04zX: %s", text->base_address + pc, "    ");

        if (instr->size == 1)
        {
            fl_cstring_vappend(&output, "%s", instr->format);
        }
        else if (instr->size == 2)
        {
            fl_cstring_vappend(&output, instr->format, text->bytes[pc + 1]);
        }
        else
        {
            fl_cstring_vappend(&output, instr->format, text->bytes[pc + 2], text->bytes[pc + 1]);
        }

        fl_cstring_vappend(&output, "%s", "\n");

        pc += instr->size;
    }
    fl_cstring_vappend(&output, "%s", "\n");

    return output;
}
