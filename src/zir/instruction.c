#include <stdio.h>
#include "instruction.h"

struct ZenitIrInstruction* zenit_ir_instruction_new(enum ZenitIrInstructionType type)
{
    struct ZenitIrInstruction *instruction = NULL;
    
    switch (type)
    {
        case ZENIT_IR_INSTR_VARIABLE:
            instruction = fl_malloc(sizeof(struct ZenitIrVariableInstruction));
            instruction->type = ZENIT_IR_INSTR_VARIABLE;
            break;
    }

    return instruction;
}

void zenit_ir_instruction_free(struct ZenitIrInstruction *instruction)
{
    switch (instruction->type)
    {
        case ZENIT_IR_INSTR_VARIABLE:
        {
            struct ZenitIrVariableInstruction *vardecl = (struct ZenitIrVariableInstruction*)instruction;
            zenit_ir_operand_free(&vardecl->lvalue);
            zenit_ir_operand_free(&vardecl->rvalue);
            zenit_ir_attribute_map_free(&vardecl->attributes);
            break;
        }
    }

    fl_free(instruction);
}

char* zenit_ir_instruction_dump(struct ZenitIrInstruction *instruction, char *output)
{
    switch (instruction->type)
    {
        case ZENIT_IR_INSTR_VARIABLE:
        {
            struct ZenitIrVariableInstruction *vardecl = (struct ZenitIrVariableInstruction*)instruction;

            fl_cstring_vappend(&output, "@%s : %s = ", vardecl->lvalue.operand.symbol->name, zenit_ir_type_to_string(&vardecl->lvalue.operand.symbol->typeinfo));
            output = zenit_ir_operand_dump(&vardecl->rvalue, output);

            if (zenit_ir_attribute_map_length(&vardecl->attributes) == 0)
                break;

            fl_cstring_append(&output, " ; ");

            const char **attr_names = zenit_ir_attribute_map_keys(&vardecl->attributes);
            size_t attr_count = fl_array_length(attr_names);
            for (size_t i=0; i < attr_count; i++)
            {
                struct ZenitIrAttribute *attr = zenit_ir_attribute_map_get(&vardecl->attributes, attr_names[i]);
                fl_cstring_vappend(&output, "#%s", attr->name);

                if (zenit_ir_property_map_length(&attr->properties) == 0)
                    continue;

                fl_cstring_append(&output, "(");
                
                const char **prop_names = zenit_ir_property_map_keys(&attr->properties);
                size_t prop_count = fl_array_length(prop_names);
                for (size_t j=0; j < prop_count; j++)
                {
                    struct ZenitIrProperty *prop = zenit_ir_property_map_get(&attr->properties, prop_names[j]);
                    fl_cstring_vappend(&output, "%s:", prop->name);

                    output = zenit_ir_operand_dump(&prop->value, output);

                    if (j != prop_count - 1)
                        fl_cstring_append(&output, ", ");
                }

                fl_array_free(prop_names);

                fl_cstring_append(&output, ")");

                if (i != attr_count - 1)
                    fl_cstring_append(&output, ", ");
            }

            fl_array_free(attr_names);

            break;
        }
    }

    fl_cstring_append(&output, "\n");

    return output;
}
