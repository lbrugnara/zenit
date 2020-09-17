#ifndef ZNES_PROGDESC_H
#define ZNES_PROGDESC_H

#include <fllib/Mem.h>
#include <fllib/Cstring.h>
#include <fllib/containers/Hashtable.h>
#include "segments/data.h"
#include "segments/text.h"
#include "segments/zp.h"
#include "instructions/alloc.h"
#include "objects/alloc.h"
#include "objects/temp.h"
#include "operands/operand.h"
#include "operands/pool.h"

typedef struct ZnesProgram {
    ZnesDataSegment *data;
    ZnesTextSegment *startup;
    ZnesTextSegment *code;
    ZnesZeroPageSegment *zp;
    ZnesVariableMap *variables;
    ZnesOperandPool *operands;
    bool startup_context;
} ZnesProgram;

static inline ZnesProgram* znes_program_new(bool scripting)
{
    ZnesProgram *program = fl_malloc(sizeof(ZnesProgram));

    program->startup_context = !scripting;
    program->data = znes_data_segment_new(0x8000);
    program->startup = znes_text_segment_new(0x0);
    program->code = znes_text_segment_new(0x0);
    program->zp = znes_zp_segment_new();
    program->operands = znes_operand_pool_new();

    program->variables = fl_hashtable_new_args((struct FlHashtableArgs) {
        .hash_function = fl_hashtable_hash_string,
        .key_allocator = fl_container_allocator_string,
        .key_comparer = fl_container_equals_string,
        .key_cleaner = fl_container_cleaner_pointer,
        .value_cleaner = (FlContainerCleanupFn) znes_alloc_free,
        .value_allocator = NULL
    });

    return program;
}

static inline void znes_program_free(ZnesProgram *program)
{
    znes_data_segment_free(program->data);
    znes_text_segment_free(program->code);
    znes_text_segment_free(program->startup);
    znes_zp_segment_free(program->zp);
    fl_hashtable_free(program->variables);
    znes_operand_pool_free(program->operands);

    fl_free(program);
}

static ZnesAlloc* znes_program_alloc_variable(ZnesProgram *program, const char *name, ZnesAllocRequest *alloc, ZnesOperand *source)
{
    ZnesAlloc *variable = NULL;

    switch (alloc->segment)
    {
        case ZNES_SEGMENT_TEMP:
        {
            ZnesTempAlloc *temp = znes_temp_alloc_new(name, alloc->size);
            temp->source = source;
            variable = (ZnesAlloc*) temp;
            break;
        }   
        case ZNES_SEGMENT_ZP:
            variable = znes_zp_segment_alloc_variable(program->zp, name, alloc, source);
            break;

        case ZNES_SEGMENT_DATA:
            variable = znes_data_segment_alloc_variable(program->data, name, alloc, source);
            break;

        case ZNES_SEGMENT_TEXT:
            variable = znes_text_segment_alloc_variable(program->startup_context ? program->startup : program->code, name, alloc, source);
            break;
        
        default: break;
    }

    if (variable != NULL)
    {
        fl_hashtable_add(program->variables, variable->name, variable);

        ZnesAllocInstruction *instr = znes_alloc_instruction_new(variable, source);
        znes_text_segment_add_instr(program->startup_context ? program->startup : program->code, (ZnesInstruction*) instr);
    }

    return variable;
}

static inline void znes_program_emit_instruction(ZnesProgram *program, ZnesInstruction *instruction)
{
    znes_text_segment_add_instr(program->startup_context ? program->startup : program->code, instruction);
}

static inline char* znes_program_dump(ZnesProgram *program)
{
    char *output = fl_cstring_new(0);

    fl_cstring_append(&output, "; PROGRAM BUILDER\n");
    output = znes_zp_segment_dump(program->zp, output);
    fl_cstring_append(&output, "\n; Startup routine\n");
    output = znes_text_segment_dump(program->startup, output);
    fl_cstring_append(&output, "\n");
    output = znes_data_segment_dump(program->data, output);
    fl_cstring_append(&output, "\n; Main routine\n");
    output = znes_text_segment_dump(program->code, output);

    return output;
}

#endif /* ZNES_PROGDESC_H */
