
#include <fllib/Cstring.h>
#include "attribute.h"

ZirAttribute* zir_attribute_new(char *name)
{
    ZirAttribute *zir_attr = fl_malloc(sizeof(ZirAttribute));
    zir_attr->name = fl_cstring_dup(name);
    zir_attr->properties = zir_property_map_new();

    return zir_attr;
}

void zir_attribute_free(ZirAttribute *attribute)
{
    if (!attribute)
        return;

    if (attribute->name)
        fl_cstring_free(attribute->name);

    zir_property_map_free(attribute->properties);
        
    fl_free(attribute);
}
