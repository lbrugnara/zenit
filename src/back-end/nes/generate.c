#include <stdint.h>
#include <fllib.h>
#include "generate.h"
#include "nesrom.h"

#include "../../zir/symtable.h"

typedef void(*ZenitIrBlockVisitor)(struct ZenitIrBlock *block, struct ZenitNesRom *rom);
typedef void(*ZenitIrInstructionVisitor)(struct ZenitIrInstruction *instruction, struct ZenitIrBlock *block, struct ZenitNesRom *rom);

static void visit_block(struct ZenitIrBlock *block, struct ZenitNesRom *rom);
static void visit_instruction(struct ZenitIrInstruction *instruction, struct ZenitIrBlock *block, struct ZenitNesRom *rom);
static void visit_variable_instruction(struct ZenitIrInstruction *instruction, struct ZenitIrBlock *block, struct ZenitNesRom *rom);

static const ZenitIrInstructionVisitor instruction_visitors[] = {
    // These nodes introduce new symbols into the symbol table or retrieve symbols from it
    [ZENIT_IR_INSTR_VARIABLE]   = &visit_variable_instruction
    
};

static void visit_variable_instruction(struct ZenitIrVariableInstruction *instruction, struct ZenitIrBlock *block, struct ZenitNesRom *rom)
{
    struct ZenitIrSymbol *symbol = instruction->lvalue.operand.symbol;

    struct ZenitIrAttribute **attributes = instruction->attributes;
}

void visit_instruction(struct ZenitIrInstruction *instruction, struct ZenitIrBlock *block, struct ZenitNesRom *rom)
{
    return instruction_visitors[instruction->type](instruction, block, rom);
}

void visit_block(struct ZenitIrBlock *block, struct ZenitNesRom *rom)
{
    size_t instr_count = fl_array_length(block->instructions);

    for (size_t i=0; i < instr_count; i++)
    {
        struct ZenitIrInstruction *instr = block->instructions[instr_count];
        visit_instruction(instr, block, rom);
    }
}

void zenit_nes_generate(struct ZenitIrProgram *program)
{
    if (!program)
        return NULL;

    struct ZenitNesRom *rom = zenit_nes_rom_new();

    struct ZenitIrBlock *block = program->current;
    visit_block(block, rom);
    
    return NULL;
}
