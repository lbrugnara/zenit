#include "attribute.h"

void zenit_ir_attribute_free(struct ZenitIrAttribute *attribute)
{
    if (!attribute)
        return;

    if (attribute->name)
        fl_cstring_free(attribute->name);

    zenit_ir_property_map_free(&attribute->properties);
        
    fl_free(attribute);
}
