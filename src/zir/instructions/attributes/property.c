#include <fllib.h>
#include "property.h"

struct ZirProperty* zir_property_new(char *name, struct ZirOperand *value)
{
    struct ZirProperty *property = fl_malloc(sizeof(struct ZirProperty));
    property->name = fl_cstring_dup(name);
    property->value = value;

    return property;
}

void zir_property_free(struct ZirProperty *property)
{
    if (!property)
        return;

    if (property->name)
        fl_cstring_free(property->name);

    fl_free(property);
}
