
#include <fllib/Mem.h>
#include <fllib/Array.h>
#include <fllib/Cstring.h>
#include "struct.h"
#include "../../types/struct.h"

static void member_free(void *ptr)
{
    if (!ptr)
        return;

    ZirStructOperandMember *member = (ZirStructOperandMember*) ptr;
    
    if (member->name)
        fl_cstring_free(member->name);

    fl_free(member);
}

ZirStructOperand* zir_operand_struct_new(ZirStructType *type)
{
    ZirStructOperand *operand = fl_malloc(sizeof(ZirStructOperand));
    operand->base.type = ZIR_OPERAND_STRUCT;
    operand->members = fl_array_new(sizeof(ZirOperand*), 0);
    operand->type = type;

    return operand;
}

void zir_operand_struct_add_member(ZirStructOperand *struct_operand, const char *name, ZirOperand *member_operand)
{
    ZirStructOperandMember *member = fl_malloc(sizeof(ZirStructOperandMember));
    member->name = fl_cstring_dup(name);
    member->operand = member_operand;
    struct_operand->members = fl_array_append(struct_operand->members, &member);
}

void zir_operand_struct_free(ZirStructOperand *struct_operand)
{
    if (!struct_operand)
        return;

    fl_array_free_each_pointer(struct_operand->members, member_free);

    if (struct_operand->type)
        zir_type_struct_free(struct_operand->type);

    fl_free(struct_operand);
}

char* zir_operand_struct_dump(ZirStructOperand *struct_operand, char *output)
{
    fl_cstring_append(&output, "{ ");
    
    size_t length = struct_operand->members ? fl_array_length(struct_operand->members) : 0;
    if (length > 0)
    {
        for (size_t i=0; i < length; i++)
        {
            if (i > 0)
                fl_cstring_append(&output, ", ");

            ZirStructOperandMember *member = struct_operand->members[i];
            fl_cstring_vappend(&output, "%s: ", member->name);
            output = zir_operand_dump(member->operand, output);
        }
        fl_cstring_append(&output, " ");
    }

    fl_cstring_append(&output, "}");

    return output;
}

char* zir_operand_struct_type_dump(ZirStructOperand *struct_operand, char *output)
{
    fl_cstring_vappend(&output, "%s", zir_type_struct_to_string(struct_operand->type));
    return output;
}

