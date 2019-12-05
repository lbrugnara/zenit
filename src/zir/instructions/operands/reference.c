#include <fllib.h>
#include "reference.h"

struct ZirReferenceOperand* zir_operand_reference_new(struct ZirOperand *operand)
{
    flm_assert(operand != NULL, "Operand of a reference must not be NULL");

    struct ZirReferenceOperand *reference = fl_malloc(sizeof(struct ZirReferenceOperand));
    reference->base.type = ZIR_OPERAND_REFERENCE;
    reference->operand = operand;

    zir_type_copy(&reference->base.typeinfo, &operand->typeinfo);
    reference->base.typeinfo.is_ref = true;

    return reference;
}

void zir_operand_reference_free(struct ZirReferenceOperand *reference)
{
    if (!reference)
        return;

    zir_type_free(&reference->base.typeinfo);

    fl_free(reference);
}

char* zir_operand_reference_dump(struct ZirReferenceOperand *reference, char *output)
{
    fl_cstring_append(&output, "ref ");
    output = zir_operand_dump(reference->operand, output);
    return output;
}
