#include "program.h"
#include "symbol.h"

struct ZenitIrProgram* zenit_ir_program_new()
{
    struct ZenitIrProgram *program = fl_malloc(sizeof(struct ZenitIrProgram));
    program->global = zenit_ir_block_new("global", ZENIT_IR_SYMTABLE_GLOBAL, NULL);
    program->current = program->global;

    return program;
}

void zenit_ir_program_free(struct ZenitIrProgram *program)
{
    if (!program)
        return;
        
    zenit_ir_block_free(program->global);

    fl_free(program);
}

struct ZenitIrSymbol* zenit_ir_program_add_global_symbol(struct ZenitIrProgram *program, struct ZenitIrSymbol *symbol)
{
    return zenit_ir_symtable_add(&program->global->symtable, symbol);
}

struct ZenitIrSymbol* zenit_ir_program_add_symbol(struct ZenitIrProgram *program, struct ZenitIrSymbol *symbol)
{
    return zenit_ir_symtable_add(&program->current->symtable, symbol);
}

struct ZenitIrInstruction* zenit_ir_program_add_instruction(struct ZenitIrProgram *program, struct ZenitIrInstruction *instruction)
{
    program->current->instructions = fl_array_append(program->current->instructions, &instruction);
    return instruction;
}

char* zenit_ir_program_dump(struct ZenitIrProgram *program, char *output)
{
    return zenit_ir_block_dump(program->global, output);
}
