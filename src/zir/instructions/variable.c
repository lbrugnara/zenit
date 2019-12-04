#include <stdio.h>
#include "variable.h"

struct ZenitIrVariableInstruction* zenit_ir_instruction_variable_new(void)
{
    struct ZenitIrVariableInstruction *instruction = fl_malloc(sizeof(struct ZenitIrVariableInstruction));
    instruction->base.type = ZENIT_IR_INSTR_VARIABLE;
    
    return instruction;
}

void zenit_ir_instruction_variable_free(struct ZenitIrVariableInstruction *instruction)
{
    struct ZenitIrVariableInstruction *vardecl = (struct ZenitIrVariableInstruction*)instruction;
    zenit_ir_operand_free(&vardecl->lvalue);
    zenit_ir_operand_free(&vardecl->rvalue);
    zenit_ir_attribute_map_free(&vardecl->attributes);

    fl_free(instruction);
}

char* zenit_ir_instruction_variable_dump(struct ZenitIrVariableInstruction *vardecl, char *output)
{
    fl_cstring_vappend(&output, "%s%s : %s = ", 
        vardecl->lvalue.operand.symbol->temporal ? "%" : "@", 
        vardecl->lvalue.operand.symbol->name, 
        zenit_ir_type_to_string(&vardecl->lvalue.operand.symbol->typeinfo)
    );
    output = zenit_ir_operand_dump(&vardecl->rvalue, output);

    if (zenit_ir_attribute_map_length(&vardecl->attributes) != 0)
    {
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

    }

    fl_cstring_append(&output, "\n");

    return output;
}
