
#include <fllib/Cstring.h>
#include "property.h"

ZirProperty* zir_property_new(char *name, ZirOperand *value)
{
    ZirProperty *property = fl_malloc(sizeof(ZirProperty));
    property->name = fl_cstring_dup(name);
    property->value = value;

    return property;
}

void zir_property_free(ZirProperty *property)
{
    if (!property)
        return;

    if (property->name)
        fl_cstring_free(property->name);

    fl_free(property);
}
