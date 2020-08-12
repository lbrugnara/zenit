
#include <fllib/Mem.h>
#include <fllib/Array.h>
#include <fllib/Cstring.h>
#include "struct.h"

static void member_free(void *ptr)
{
    if (!ptr)
        return;

    ZnesStructOperandMember *member = (ZnesStructOperandMember*) ptr;
    
    if (member->name)
        fl_cstring_free(member->name);

    fl_free(member);
}

ZnesStructOperand* znes_struct_operand_new(void)
{
    ZnesStructOperand *operand = fl_malloc(sizeof(ZnesStructOperand));
    operand->base.type = ZNES_OPERAND_STRUCT;
    operand->members = fl_array_new(sizeof(ZnesOperand*), 0);

    return operand;
}

void znes_struct_operand_add_member(ZnesStructOperand *struct_operand, const char *name, ZnesOperand *member_operand)
{
    ZnesStructOperandMember *member = fl_malloc(sizeof(ZnesStructOperandMember));
    member->name = fl_cstring_dup(name);
    member->operand = member_operand;
    struct_operand->members = fl_array_append(struct_operand->members, &member);
}

void znes_struct_operand_free(ZnesStructOperand *struct_operand)
{
    if (!struct_operand)
        return;

    fl_array_free_each_pointer(struct_operand->members, member_free);

    fl_free(struct_operand);
}

size_t znes_struct_operand_size(ZnesStructOperand *struct_operand)
{
    // TODO: Handle overflow
    size_t size = 0;

    for (size_t i = 0; i < fl_array_length(struct_operand->members); i++)
    {
        ZnesStructOperandMember *member = struct_operand->members[i];
        size += znes_operand_size(member->operand);
    }

    return size;
}

char* znes_struct_operand_dump(ZnesStructOperand *struct_operand, char *output)
{
    fl_cstring_append(&output, "{ ");
    
    size_t length = struct_operand->members ? fl_array_length(struct_operand->members) : 0;
    if (length > 0)
    {
        for (size_t i=0; i < length; i++)
        {
            if (i > 0)
                fl_cstring_append(&output, ", ");

            ZnesStructOperandMember *member = struct_operand->members[i];
            fl_cstring_vappend(&output, "%s: ", member->name);
            output = znes_operand_dump(member->operand, output);
        }
        fl_cstring_append(&output, " ");
    }

    fl_cstring_append(&output, "}");

    return output;
}
