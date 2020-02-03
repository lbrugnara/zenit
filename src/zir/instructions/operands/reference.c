
#include <fllib/Cstring.h>
#include "reference.h"

struct ZirReferenceOperand* zir_operand_reference_new(struct ZirReferenceType *type, struct ZirSymbolOperand *operand)
{
    flm_assert(operand != NULL, "Operand of a reference must not be NULL");

    struct ZirReferenceOperand *reference = fl_malloc(sizeof(struct ZirReferenceOperand));
    reference->base.type = ZIR_OPERAND_REFERENCE;
    reference->operand = operand;
    reference->type = type;

    return reference;
}

void zir_operand_reference_free(struct ZirReferenceOperand *reference)
{
    if (!reference)
        return;

    if (reference->type)
        zir_type_reference_free(reference->type);

    fl_free(reference);
}

char* zir_operand_reference_dump(struct ZirReferenceOperand *reference, char *output)
{
    fl_cstring_append(&output, "ref ");
    output = zir_operand_symbol_dump(reference->operand, output);
    return output;
}

char* zir_operand_reference_type_dump(struct ZirReferenceOperand *reference, char *output)
{
    fl_cstring_vappend(&output, "%s", zir_type_reference_to_string(reference->type));
    return output;
}
