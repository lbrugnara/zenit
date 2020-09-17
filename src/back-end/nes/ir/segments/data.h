#ifndef ZNES_DATA_SEGDESC_H
#define ZNES_DATA_SEGDESC_H

#include <fllib/Mem.h>
#include <fllib/containers/List.h>
#include "../instructions/alloc.h"
#include "../operands/operand.h"
#include "../objects/alloc.h"

typedef struct ZnesDataSegment {
    ZnesAllocInstructionList *stores;
    uint16_t base_address;
    size_t used;
} ZnesDataSegment;

static inline ZnesDataSegment* znes_data_segment_new(uint16_t base_address)
{
    ZnesDataSegment *data = fl_malloc(sizeof(ZnesDataSegment));

    data->stores = znes_alloc_instruction_list_new();
    data->base_address = base_address;
    data->used = base_address;

    return data;
}

static inline void znes_data_segment_free(ZnesDataSegment *data)
{
    znes_alloc_instruction_list_free(data->stores);

    fl_free(data);
}

static inline ZnesAlloc* znes_data_segment_alloc_variable(ZnesDataSegment *data, const char *name, ZnesAllocRequest *alloc, ZnesOperand *source)
{
    size_t needed_space = alloc->size;

    /*// If there is no available space to store the symbol, we can't do anything
    if (data->used + needed_space > ???DATA_SIZE)
        return false;

    // If the address is outside of DATA or the element does not fit, we can't do anything
    if (address != NULL && (*address > ???DATA_SIZE || *address + needed_space > ???DATA_SIZE))
        return false;    */

    ZnesAlloc *nes_symbol = NULL;
    struct FlListNode *head = fl_list_head(data->stores);

    if (head == NULL)
    {
        // When there are no symbols, we can directly insert the symbol
        nes_symbol = znes_alloc_new(alloc->type, name, ZNES_SEGMENT_DATA, alloc->size, alloc->use_address ? alloc->address : data->base_address);
        fl_list_append(data->stores, znes_alloc_instruction_new(nes_symbol, source));
        data->used += needed_space;

        return nes_symbol;
    }    

    bool already_exist = false;
    struct FlListNode *node = head;
    if (!alloc->use_address)
    {
        uint16_t probe_address = data->base_address;
        while (node)
        {
            ZnesAlloc *symbol = ((ZnesAllocInstruction*) node->value)->destination;

            // When the address for the new symbol is not provided, we can store the symbol in the first place it "fits", that
            // can be:
            //  a)  We are at the last element of the list, so it is safe to place the symbol at the end (calculating the address)
            //  b)  There is space between the last symbol we visited (node->prev) and the current symbol being visited
            bool fits_at_the_end = node->next == NULL;

            // If the address overflows, we break the loop
            if (fl_std_uint_add_overflow(probe_address, needed_space, UINT16_MAX))
                break;

            bool fits_before = probe_address + needed_space <= symbol->address;

            if (fits_before || fits_at_the_end)
            {
                size_t symbol_address = fits_at_the_end ? symbol->address + symbol->size : probe_address;
                nes_symbol = znes_alloc_new(alloc->type, name, ZNES_SEGMENT_DATA, alloc->size, symbol_address);

                if (fits_at_the_end)
                    fl_list_append(data->stores, znes_alloc_instruction_new(nes_symbol, source));
                else
                    fl_list_insert_before(data->stores, node, znes_alloc_instruction_new(nes_symbol, source));

                break;
            }

            // If the address overflows, we break the loop
            if (fl_std_uint_add_overflow(symbol->address, symbol->size, UINT16_MAX))
                break;

            // "Move forward" to the next available gap
            probe_address = symbol->address + symbol->size;

            node = node->next;
        }
    }
    else
    {
        while (node)
        {
            ZnesAlloc *symbol = ((ZnesAllocInstruction*) node->value)->destination;

            // If the allocation is similar to a previous one, it might be an alias, we allow
            // this (mmm)
            if (symbol->address == alloc->address && symbol->type == alloc->type && symbol->size == alloc->size)
            {
                already_exist = true;
                nes_symbol = znes_alloc_new(alloc->type, name, ZNES_SEGMENT_DATA, alloc->size, alloc->address);
                fl_list_insert_before(data->stores, node, znes_alloc_instruction_new(nes_symbol, source));
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
                nes_symbol = znes_alloc_new(alloc->type, name, ZNES_SEGMENT_DATA, alloc->size, alloc->address);

                if (fits_at_the_end)
                    fl_list_append(data->stores, znes_alloc_instruction_new(nes_symbol, source));
                else
                    fl_list_insert_before(data->stores, node, znes_alloc_instruction_new(nes_symbol, source));

                break;
            }

            node = node->next;
        }
    }

    if (nes_symbol != NULL && !already_exist)
        data->used += needed_space;

    return nes_symbol;
}

static inline char* znes_data_segment_dump(ZnesDataSegment *data, char *output)
{
    fl_cstring_vappend(&output, "; DATA segment size: %zu byte%s (base address: 0x%02X)\n\n", data->used, (data->used > 1 ? "s":""), data->base_address);
    struct FlListNode *node = fl_list_head(data->stores);

    while (node)
    {
        ZnesAllocInstruction *instr = (ZnesAllocInstruction*) node->value;
        ZnesAlloc *symbol = instr->destination;

        fl_cstring_vappend(&output, "\t; addr: 0x%02X size: %zu byte%s\n", symbol->address, symbol->size, (symbol->size > 1 ? "s" : ""));
        fl_cstring_append(&output, "\t");
        output = znes_alloc_instruction_dump(instr, output);
        fl_cstring_vappend(&output, "%s", "\n\n");

        node = node->next;
    }

    return output;
}

#endif /* ZNES_DATA_SEGDESC_H */
