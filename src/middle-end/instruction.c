#include <stdio.h>
#include "instruction.h"

struct ZirInstruction* zir_instruction_new(enum ZirInstructionType type)
{
    struct ZirInstruction *instruction = NULL;
    
    switch (type)
    {
        case ZIR_INSTR_VARDECL:
            instruction = fl_malloc(sizeof(struct ZirVarDeclInstruction));
            instruction->type = ZIR_INSTR_VARDECL;
            break;
    }

    return instruction;
}

void zir_instruction_free(struct ZirInstruction *instruction)
{
    switch (instruction->type)
    {
        case ZIR_INSTR_VARDECL:
            struct ZirVarDeclInstruction *vardecl = (struct ZirVarDeclInstruction*)instruction;
            zir_operand_free(&vardecl->lvalue);
            zir_operand_free(&vardecl->rvalue);
            break;
    }

    fl_free(instruction);
}

char* zir_instruction_dump(struct ZirInstruction *instruction, char *output)
{
    switch (instruction->type)
    {
        case ZIR_INSTR_VARDECL:
            struct ZirVarDeclInstruction *vardecl = (struct ZirVarDeclInstruction*)instruction;

            fl_cstring_vappend(&output, "@%s : %s = ", vardecl->lvalue.operand.symbol->name, zir_type_to_string(&vardecl->lvalue.operand.symbol->typeinfo));
            output = zir_operand_dump(&vardecl->rvalue, output);
            break;
    }

    fl_cstring_append(&output, "\n");

    return output;
}
