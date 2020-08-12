#ifndef ZNES_ALLOC_INSTR_H
#define ZNES_ALLOC_INSTR_H

#include <fllib/Mem.h>
#include <fllib/containers/List.h>
#include "instr.h"
#include "../operands/operand.h"
#include "../objects/alloc.h"

typedef FlList ZnesAllocInstructionList;

typedef struct ZnesAllocInstruction {
    ZnesInstruction base;
    ZnesAlloc *destination;
    ZnesOperand *source;
} ZnesAllocInstruction;

#define znes_alloc_instruction_new_local(dest, src) ((ZnesAllocInstruction) { .base.kind = ZNES_INSTRUCTION_ALLOC, .destination = (dest), .source = (src) })

ZnesAllocInstruction* znes_alloc_instruction_new(ZnesAlloc *destination, ZnesOperand *source);
void znes_alloc_instruction_free(ZnesAllocInstruction *instruction);
char* znes_alloc_instruction_dump(ZnesAllocInstruction *instruction, char *output);

static inline ZnesAllocInstructionList* znes_alloc_instruction_list_new(void)
{
    return fl_list_new_args((struct FlListArgs) { .value_cleaner = (FlContainerCleanupFn) znes_alloc_instruction_free });
}

static inline void znes_alloc_instruction_list_free(ZnesAllocInstructionList *list)
{
    fl_list_free(list);
}

static inline void znes_alloc_instruction_list_add(ZnesAllocInstructionList *list, ZnesAllocInstruction *instr)
{
    fl_list_append(list, instr);
}


#endif /* ZNES_ALLOC_INSTR_H */
