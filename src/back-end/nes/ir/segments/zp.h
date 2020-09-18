#ifndef ZNES_ZP_SEGDESC_H
#define ZNES_ZP_SEGDESC_H

#include <stdint.h>
#include <fllib/Mem.h>
#include <fllib/containers/List.h>
#include "../instructions/alloc.h"
#include "../operands/operand.h"
#include "../objects/alloc.h"

typedef struct ZnesZeroPageSegment {
    ZnesAllocInstructionList *allocations;
    size_t used;
} ZnesZeroPageSegment;

static inline ZnesZeroPageSegment* znes_zp_segment_new(void)
{
    ZnesZeroPageSegment *zp = fl_malloc(sizeof(ZnesZeroPageSegment));

    zp->allocations = znes_alloc_instruction_list_new();
    zp->used = 0;

    return zp;
}

static inline void znes_zp_segment_free(ZnesZeroPageSegment *zp)
{
    znes_alloc_instruction_list_free(zp->allocations);

    fl_free(zp);
}

#define ZERO_PAGE_SIZE 0xFF

static inline ZnesAlloc* znes_zp_segment_alloc_variable(ZnesZeroPageSegment *zp_emitter, const char *name, ZnesAllocRequest *alloc, ZnesOperand *source)
{
    size_t needed_space = alloc->size;

    // If there is no available space to store the symbol, we can't do anything
    if (zp_emitter->used + needed_space > ZERO_PAGE_SIZE)
        return NULL;

    // If the address is outside of ZP or the element does not fit, we can't do anything
    if (alloc->use_address && (alloc->address > ZERO_PAGE_SIZE || alloc->address + needed_space > ZERO_PAGE_SIZE))
        return NULL;    


    ZnesAlloc *nes_symbol = NULL;
    struct FlListNode *head = fl_list_head(zp_emitter->allocations);

    if (head == NULL)
    {
        // When there are no symbols, we can directly insert the symbol
        nes_symbol = znes_alloc_new(alloc->type, name, ZNES_SEGMENT_ZP, alloc->size, alloc->use_address ? alloc->address : 0);
        fl_list_append(zp_emitter->allocations, znes_alloc_instruction_new(nes_symbol, source));
        zp_emitter->used += needed_space;

        return nes_symbol;
    }    

    bool already_exist = false;
    struct FlListNode *node = head;
    if (!alloc->use_address)
    {
        uint16_t probe_address = 0;
        while (node)
        {
            ZnesAlloc *symbol = ((ZnesAllocInstruction*) node->value)->destination;

            // When the address for the new symbol is not provided, we can store the symbol in the first place it "fits", that
            // can be:
            //  a)  We are at the last element of the list, so it is safe to place the symbol at the end (calculating the address)
            //  b)  There is space between the last symbol we visited (node->prev) and the current symbol being visited
            bool fits_at_the_end = node->next == NULL;

            // If the address overflows, we break the loop
            if (fl_std_uint_add_overflow(probe_address, needed_space, UINT8_MAX))
                break;

            bool fits_before = probe_address + needed_space <= symbol->address;

            if (fits_before || fits_at_the_end)
            {
                uint16_t symbol_address = fits_at_the_end ? symbol->address + symbol->size : probe_address;
                nes_symbol = znes_alloc_new(alloc->type, name, ZNES_SEGMENT_ZP, alloc->size, symbol_address);

                if (fits_at_the_end)
                    fl_list_append(zp_emitter->allocations, znes_alloc_instruction_new(nes_symbol, source));
                else
                    fl_list_insert_before(zp_emitter->allocations, node, znes_alloc_instruction_new(nes_symbol, source));

                break;
            }

            // If the address overflows, we break the loop
            if (fl_std_uint_add_overflow(symbol->address, symbol->size, UINT8_MAX))
                break;

            // "Move forward" to the next available gap
            probe_address = symbol->address + symbol->size;

            node = node->next;
        }
    }
    else
    {
        // TODO: Currently we allow aliasing if the allocation size and type is equals to a previously allocated one.
        // This is really useful for special memory addresses like PPUSTATUS, or reading from joypads. That being said, it could be
        // useful to alias different allocations size for temporal values, so we could use a flag or an attribute property to 
        // allow/disable aliasing.
        while (node)
        {
            ZnesAlloc *symbol = ((ZnesAllocInstruction*) node->value)->destination;

            // If the allocation is similar to a previous one, it might be an alias, we allow
            // this (mmm)
            if (symbol->address == alloc->address && symbol->type == alloc->type && symbol->size == alloc->size)
            {
                already_exist = true;
                nes_symbol = znes_alloc_new(alloc->type, name, ZNES_SEGMENT_ZP, alloc->size, alloc->address);
                fl_list_insert_before(zp_emitter->allocations, node, znes_alloc_instruction_new(nes_symbol, source));
                break;
            }

            // When an address is provided, the new symbol needs to be store at that particular address (if possible). Under
            // this constraint, there are 2 places where the symbol can fit:
            //  a)  The current symbol in the list has an address that is greater than the one we want to use for the new symbol 
            //      and the 'needed_space' is available between these symbols so that we insert the new symbol before the current 
            //      symbol in the list
            //  b)  The desired address is greater than the current symbol's address and there are no more symbols in the list
            //      so that we place the new symbol at the end of the list
            bool fits_at_the_end = alloc->address >= symbol->address + symbol->size && node->next == NULL;
            bool fits_before = alloc->address + needed_space <= symbol->address;            

            if (fits_before || fits_at_the_end)
            {
                nes_symbol = znes_alloc_new(alloc->type, name, ZNES_SEGMENT_ZP, alloc->size, alloc->address);

                if (fits_at_the_end)
                    fl_list_append(zp_emitter->allocations, znes_alloc_instruction_new(nes_symbol, source));
                else
                    fl_list_insert_before(zp_emitter->allocations, node, znes_alloc_instruction_new(nes_symbol, source));

                break;
            }

            node = node->next;
        }
    }

    if (nes_symbol != NULL && !already_exist)
        zp_emitter->used += needed_space;

    return nes_symbol;
}

static inline char* znes_zp_segment_dump(ZnesZeroPageSegment *zp, char *output)
{
    fl_cstring_vappend(&output, "; ZP segment size: %zu byte%s\n\n", zp->used, (zp->used > 1 ? "s":""));
    struct FlListNode *node = fl_list_head(zp->allocations);

    while (node)
    {
        ZnesAlloc *symbol = ((ZnesAllocInstruction*) node->value)->destination;

        fl_cstring_vappend(&output, "\t; addr: 0x%02X size: %zu byte%s\n", symbol->address, symbol->size, (symbol->size > 1 ? "s" : ""));
        fl_cstring_vappend(&output, "\t%s\n\n", symbol->name);

        node = node->next;
    }

    return output;
}

#endif /* ZNES_ZP_SEGDESC_H */
