
#include <fllib/Cstring.h>
#include "property.h"

ZenitPropertyNode* zenit_property_node_new(ZenitSourceLocation location, char *name, ZenitNode *value)
{
    ZenitPropertyNode *property = fl_malloc(sizeof(ZenitPropertyNode));
    property->base.nodekind = ZENIT_NODE_PROPERTY;
    property->base.location = location;
    property->name = name;
    property->value = value;

    return property;
}

char* zenit_property_node_uid(ZenitPropertyNode *property)
{
    if (!property)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_property_%s", property->base.location.line, property->base.location.col, property->name);
}

char* zenit_property_node_dump(ZenitPropertyNode *property, char *output)
{
    fl_cstring_vappend(&output, "(prop %s ", property->name);
    
    output = zenit_node_dump(property->value, output);    
    
    fl_cstring_append(&output, ")");

    return output;
}

void zenit_property_node_free(ZenitPropertyNode *node)
{
    if (!node)
        return;

    if (node->name)
        fl_cstring_free(node->name);

    zenit_node_free(node->value);

    fl_free(node);
}
