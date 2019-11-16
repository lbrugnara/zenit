#include "property.h"

void zenit_ir_property_free(struct ZenitIrProperty *property)
{
    if (!property)
        return;

    if (property->name)
        fl_cstring_free(property->name);

    zenit_ir_operand_free(&property->value);

    fl_free(property);
}
