#include <inttypes.h>
#include <fllib/Mem.h>
#include <fllib/Array.h>
#include <fllib/Cstring.h>
#include "segment-data.h"
#include "instruction.h"

Rp2a03DataSegment* rp2a03_data_segment_new(uint16_t base_address, size_t size)
{
    Rp2a03DataSegment *data = fl_malloc(sizeof(Rp2a03DataSegment));

    data->base_address = base_address;
    data->bytes = fl_array_new(sizeof(uint8_t), size);
    data->slots = fl_array_new(sizeof(uint8_t), size);

    return data;
}

void rp2a03_data_segment_free(Rp2a03DataSegment *data)
{
    fl_array_free(data->bytes);
    fl_array_free(data->slots);
    fl_free(data);
}

char* rp2a03_data_segment_disassemble(Rp2a03DataSegment *data, bool as_code, char *output)
{
    size_t size = 0;

    for (size_t i=0; i < fl_array_length(data->slots); i++)
    {
        if (data->slots[i] != 0)
            size++;
    }

    if (size == 0)
        return output;

    fl_cstring_append(&output, "; DATA segment hex dump\n;\n");
    
    fl_cstring_append(&output, ";      | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    fl_cstring_append(&output, "; -----+------------------------------------------------\n");
    bool skipped = false;
    for (size_t i=0; i < size; i += 0x10)
    {
        bool used_slots = false;
        for (size_t s=0; s < 0xF && s + i < size; s++)
        {
            if (data->slots[s + i] != 0)
            {
                used_slots = true;
                break;
            }
        }
        
        if (!used_slots)
        {
            skipped = true;
            continue;
        }

        if (skipped)
        {
            fl_cstring_append(&output, "; .... |\n");
            skipped = false;
        }

        fl_cstring_vappend(&output, "; %04zX |", i);

        for (size_t j=0; j <= 0xF && j + i < size; j++)
            fl_cstring_vappend(&output, " %02"PRIx8, data->bytes[j + i]);

        fl_cstring_vappend(&output, "%s", "\n");
    }
    fl_cstring_vappend(&output, "%s", "\n");

    if (as_code)
    {
        fl_cstring_append(&output, "; DATA segment as code\n;\n");

        bool skipped = false;
        for (size_t pc = 0; pc < size;)
        {
            if (data->slots[pc] == 0)
            {
                skipped = true;
                pc++;
                continue;
            }

            if (skipped)
            {
                fl_cstring_vappend(&output, ".... %s%s", "    ", "\n");
                skipped = false;
            }

            Rp2a03Instruction *instr = rp2a03_instruction_lookup(data->bytes[pc]);

            fl_cstring_vappend(&output, "%04zX: %s", data->base_address + pc, "    ");

            if (instr->size == 1)
            {
                fl_cstring_vappend(&output, "%s", instr->format);
            }
            else if (instr->size == 2)
            {
                fl_cstring_vappend(&output, instr->format, data->bytes[pc + 1]);
            }
            else
            {
                fl_cstring_vappend(&output, instr->format, data->bytes[pc + 2], data->bytes[pc + 1]);
            }

            fl_cstring_vappend(&output, "%s", "\n");

            pc += instr->size;
        }
        fl_cstring_vappend(&output, "%s", "\n");
    }

    return output;
}
