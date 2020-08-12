#include <fllib/Mem.h>
#include <fllib/Array.h>
#include <fllib/Cstring.h>
#include "segment-text.h"
#include "instruction.h"

Rp2a03TextSegment* rp2a03_text_segment_new(size_t base_address)
{
    Rp2a03TextSegment *text = fl_malloc(sizeof(Rp2a03TextSegment));

    text->pc = 0;
    text->bytes = fl_array_new(sizeof(uint8_t), UINT16_MAX);
    text->base_address = base_address;

    return text;
}

void rp2a03_text_segment_free(Rp2a03TextSegment *text)
{
    fl_array_free(text->bytes);
    fl_free(text);
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

        fl_cstring_vappend(&output, "%04zX: %s", text->base_address + pc, "\t");

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
