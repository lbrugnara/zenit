
#include <fllib/Cstring.h>
#include "reference.h"

ZnesReferenceOperand* znes_reference_operand_new(ZnesVariableOperand *operand)
{
    flm_assert(operand != NULL, "Operand of a reference must not be NULL");

    ZnesReferenceOperand *reference = fl_malloc(sizeof(ZnesReferenceOperand));
    reference->base.type = ZNES_OPERAND_REFERENCE;
    reference->operand = operand;

    return reference;
}

void znes_reference_operand_free(ZnesReferenceOperand *reference)
{
    if (!reference)
        return;

    fl_free(reference);
}

char* znes_reference_operand_dump(ZnesReferenceOperand *reference, char *output)
{
    fl_cstring_append(&output, "ref ");
    output = znes_variable_operand_dump(reference->operand, output);
    return output;
}
