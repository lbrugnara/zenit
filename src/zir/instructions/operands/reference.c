
#include <fllib/Cstring.h>
#include "reference.h"

ZirReferenceOperand* zir_reference_operand_new(ZirReferenceType *type, ZirSymbolOperand *operand)
{
    flm_assert(operand != NULL, "Operand of a reference must not be NULL");

    ZirReferenceOperand *reference = fl_malloc(sizeof(ZirReferenceOperand));
    reference->base.type = ZIR_OPERAND_REFERENCE;
    reference->operand = operand;
    reference->type = type;

    return reference;
}

void zir_reference_operand_free(ZirReferenceOperand *reference)
{
    if (!reference)
        return;

    if (reference->type)
        zir_reference_type_free(reference->type);

    fl_free(reference);
}

char* zir_reference_operand_dump(ZirReferenceOperand *reference, char *output)
{
    fl_cstring_append(&output, "ref ");
    output = zir_symbol_operand_dump(reference->operand, output);
    return output;
}

char* zir_reference_operand_type_dump(ZirReferenceOperand *reference, char *output)
{
    fl_cstring_vappend(&output, "%s", zir_reference_type_to_string(reference->type));
    return output;
}
