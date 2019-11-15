#include <stdio.h>
#include "instruction.h"

static void free_attribute_property(void *ptr)
{
    if (!ptr)
        return;

    struct ZirAttributeProperty *prop = (struct ZirAttributeProperty*)ptr;

    if (!prop)
        return;

    if (prop->name)
        fl_cstring_free(prop->name);

    zir_operand_free(&prop->value);
}

static inline void free_attribute(struct ZirAttribute *attribute)
{
    if (!attribute)
        return;

    if (attribute->name)
        fl_cstring_free(attribute->name);

    if (attribute->properties)
        fl_array_free_each(attribute->properties, free_attribute_property);
        
    fl_free(attribute);
}

static void free_attribute_array_entry(void *ptr)
{
    if (!ptr)
        return;

    struct ZirAttribute *attr = *(struct ZirAttribute**)ptr;

    if (!attr)
        return;

    free_attribute(attr);
}

struct ZirInstruction* zir_instruction_new(enum ZirInstructionType type)
{
    struct ZirInstruction *instruction = NULL;
    
    switch (type)
    {
        case ZIR_INSTR_VARDECL:
            instruction = fl_malloc(sizeof(struct ZirVariableInstruction));
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
            struct ZirVariableInstruction *vardecl = (struct ZirVariableInstruction*)instruction;
            zir_operand_free(&vardecl->lvalue);
            zir_operand_free(&vardecl->rvalue);
            if (vardecl->attributes)
                fl_array_free_each(vardecl->attributes, free_attribute_array_entry);
            break;
    }

    fl_free(instruction);
}

char* zir_instruction_dump(struct ZirInstruction *instruction, char *output)
{
    switch (instruction->type)
    {
        case ZIR_INSTR_VARDECL:
            struct ZirVariableInstruction *vardecl = (struct ZirVariableInstruction*)instruction;

            fl_cstring_vappend(&output, "@%s : %s = ", vardecl->lvalue.operand.symbol->name, zir_type_to_string(&vardecl->lvalue.operand.symbol->typeinfo));
            output = zir_operand_dump(&vardecl->rvalue, output);

            if (vardecl->attributes)
            {
                fl_cstring_append(&output, " ; ");

                size_t attr_count = fl_array_length(vardecl->attributes);
                for (size_t i=0; i < attr_count; i++)
                {
                    struct ZirAttribute *attr = vardecl->attributes[i];
                    fl_cstring_vappend(&output, "#%s", attr->name);

                    if (attr->properties)
                    {
                        fl_cstring_append(&output, "(");
                        size_t prop_count = fl_array_length(attr->properties);
                        for (size_t j=0; j < prop_count; j++)
                        {
                            struct ZirAttributeProperty prop = attr->properties[j];
                            fl_cstring_vappend(&output, "%s:", prop.name);

                            output = zir_operand_dump(&prop.value, output);

                            if (j != prop_count - 1)
                                fl_cstring_append(&output, ", ");
                        }
                        fl_cstring_append(&output, ")");
                    }

                    if (i != attr_count - 1)
                        fl_cstring_append(&output, ", ");
                }
            }
            break;
    }

    fl_cstring_append(&output, "\n");

    return output;
}
