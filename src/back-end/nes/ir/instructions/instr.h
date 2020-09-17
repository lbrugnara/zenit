#ifndef ZNES_INSTRUCTION_H
#define ZNES_INSTRUCTION_H

#include <fllib/Mem.h>
#include <fllib/Cstring.h>
#include <fllib/containers/List.h>
#include "../../../../zir/instructions/operands/operand.h"

typedef FlList ZnesInstructionList;
typedef struct FlListNode ZnesInstructionListNode;

typedef enum ZnesInstructionKind {
    ZNES_INSTRUCTION_UNK,
    ZNES_INSTRUCTION_ALLOC,
    ZNES_INSTRUCTION_IF_FALSE,
    ZNES_INSTRUCTION_JUMP,
} ZnesInstructionKind;

typedef struct ZnesInstruction {
    ZnesInstructionKind kind;
} ZnesInstruction;

void znes_instruction_free(ZnesInstruction *instr_builder);
char* znes_instruction_dump(ZnesInstruction *instr_builder, char *output);

static inline ZnesInstructionListNode* znes_instruction_list_head(ZnesInstructionList *list)
{
    return fl_list_head(list);
}

static inline ZnesInstructionList* znes_instruction_list_new(void)
{
    return fl_list_new_args((struct FlListArgs) { .value_cleaner = (FlContainerCleanupFn) znes_instruction_free });
}

static inline void znes_instruction_list_free(ZnesInstructionList *list)
{
    fl_list_free(list);
}

static inline void znes_instruction_list_add(ZnesInstructionList *list, ZnesInstruction *instr)
{
    fl_list_append(list, instr);
}

static inline char* znes_instruction_list_dump(ZnesInstructionList *list, char *output)
{
    struct FlListNode *node = fl_list_head(list);

    fl_cstring_vappend(&output, "; Number of instructions: %zu\n", fl_list_length(list));

    while (node)
    {
        ZnesInstruction *instrbuilder = (ZnesInstruction*) node->value;

        output = znes_instruction_dump(instrbuilder, output);
        fl_cstring_vappend(&output, "%s", "\n");

        node = node->next;
    }

    return output;
}

#endif /* ZNES_INSTRUCTION_H */
