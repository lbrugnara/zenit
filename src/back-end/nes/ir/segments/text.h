#ifndef ZNES_TEXT_SEGDESC_H
#define ZNES_TEXT_SEGDESC_H

#include <stdint.h>
#include <fllib/Mem.h>
#include "../instructions/instr.h"
#include "../objects/alloc.h"
#include "../operands/operand.h"

typedef struct ZnesTextSegment {
    ZnesInstructionList *instructions;
    ZnesAllocList *allocations;
    uint16_t base_address;
    size_t used;
} ZnesTextSegment;

static inline ZnesTextSegment* znes_text_segment_new(size_t base_address)
{
    ZnesTextSegment *text = fl_malloc(sizeof(ZnesTextSegment));

    text->instructions = znes_instruction_list_new();
    text->allocations = fl_list_new();
    text->base_address = base_address;
    text->used = 0;

    return text;
}

static inline void znes_text_segment_free(ZnesTextSegment *text)
{
    znes_instruction_list_free(text->instructions);
    fl_list_free(text->allocations);

    fl_free(text);
}

static inline void znes_text_segment_add_instr(ZnesTextSegment *text, ZnesInstruction *instr)
{
    znes_instruction_list_add(text->instructions, instr);
}

static inline ZnesAlloc* znes_text_segment_alloc_variable(ZnesTextSegment *text, const char *name, ZnesAllocRequest *alloc, ZnesOperand *source)
{
    // TODO: By now we use the address
    ZnesAlloc *nes_symbol = znes_alloc_new(alloc->type, name, ZNES_SEGMENT_TEXT, alloc->size, alloc->address);
    //fl_list_append(text->instructions, znes_alloc_instruction_new(nes_symbol, source));
    fl_list_append(text->allocations, nes_symbol);
    text->used += alloc->size;

    return nes_symbol;
}

static inline char* znes_text_segment_dump(ZnesTextSegment *text, char *output)
{
    fl_cstring_vappend(&output, "; Allocations: %zu\n\n", fl_list_length(text->allocations));

    struct FlListNode *node = fl_list_head(text->allocations);

    while (node)
    {
        ZnesAlloc *variable = (ZnesAlloc*) node->value;

        fl_cstring_vappend(&output, "\t; addr: 0x%02X\n", variable->address);
        fl_cstring_vappend(&output, "\t%s\n\n", variable->name);
        node = node->next;
    }

    fl_cstring_vappend(&output, "; Number of instructions: %zu\n\n", fl_list_length(text->instructions));

    node = fl_list_head(text->instructions);

    while (node)
    {
        ZnesInstruction *instrbuilder = (ZnesInstruction*) node->value;

        fl_cstring_vappend(&output, "%s", "\t");
        output = znes_instruction_dump(instrbuilder, output);
        fl_cstring_vappend(&output, "%s", "\n");

        node = node->next;
    }

    return output;
}


#endif /* ZNES_TEXT_SEGDESC_H */
