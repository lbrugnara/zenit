#include "property.h"

void zir_property_free(struct ZirProperty *property)
{
    if (!property)
        return;

    if (property->name)
        fl_cstring_free(property->name);

    zir_operand_free(&property->value);

    fl_free(property);
}
