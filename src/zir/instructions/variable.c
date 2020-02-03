#include <stdio.h>
#include <fllib/Cstring.h>
#include "variable.h"

struct ZirVariableInstruction* zir_instruction_variable_new(struct ZirOperand *destination, struct ZirOperand *source)
{
    struct ZirVariableInstruction *instruction = fl_malloc(sizeof(struct ZirVariableInstruction));
    instruction->base.type = ZIR_INSTR_VARIABLE;
    instruction->base.destination = destination;
    instruction->source = source;

    return instruction;
}

void zir_instruction_variable_free(struct ZirVariableInstruction *instruction)
{
    zir_attribute_map_free(instruction->attributes);
    fl_free(instruction);
}

char* zir_instruction_variable_dump(struct ZirVariableInstruction *vardecl, char *output)
{
    output = zir_operand_dump(vardecl->base.destination, output);
    fl_cstring_append(&output, " : ");
    output = zir_operand_type_dump(vardecl->base.destination, output);
    fl_cstring_append(&output, " = ");
    output = zir_operand_dump(vardecl->source, output);

    if (zir_attribute_map_length(vardecl->attributes) != 0)
    {
        fl_cstring_append(&output, " ; ");

        const char **attr_names = zir_attribute_map_keys(vardecl->attributes);
        size_t attr_count = fl_array_length(attr_names);
        for (size_t i=0; i < attr_count; i++)
        {
            struct ZirAttribute *attr = zir_attribute_map_get(vardecl->attributes, attr_names[i]);
            fl_cstring_vappend(&output, "#%s", attr->name);

            if (zir_property_map_length(attr->properties) == 0)
                continue;

            fl_cstring_append(&output, "(");
            
            const char **prop_names = zir_property_map_keys(attr->properties);
            size_t prop_count = fl_array_length(prop_names);
            for (size_t j=0; j < prop_count; j++)
            {
                struct ZirProperty *prop = zir_property_map_get(attr->properties, prop_names[j]);
                fl_cstring_vappend(&output, "%s:", prop->name);

                output = zir_operand_dump(prop->value, output);

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
