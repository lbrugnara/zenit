#include "attribute.h"

void zir_attribute_free(struct ZirAttribute *attribute)
{
    if (!attribute)
        return;

    if (attribute->name)
        fl_cstring_free(attribute->name);

    zir_property_map_free(&attribute->properties);
        
    fl_free(attribute);
}
