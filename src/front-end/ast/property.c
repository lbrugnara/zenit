
#include <fllib/Cstring.h>
#include "property.h"

struct ZenitPropertyNode* zenit_node_property_new(struct ZenitSourceLocation location, char *name, struct ZenitNode *value)
{
    struct ZenitPropertyNode *property = fl_malloc(sizeof(struct ZenitPropertyNode));
    property->base.nodekind = ZENIT_NODE_PROPERTY;
    property->base.location = location;
    property->name = name;
    property->value = value;

    return property;
}

char* zenit_node_property_uid(struct ZenitPropertyNode *property)
{
    if (!property)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_property_%s", property->base.location.line, property->base.location.col, property->name);
}

char* zenit_node_property_dump(struct ZenitPropertyNode *property, char *output)
{
    fl_cstring_vappend(&output, "(prop %s ", property->name);
    
    output = zenit_node_dump(property->value, output);    
    
    fl_cstring_append(&output, ")");

    return output;
}

void zenit_node_property_free(struct ZenitPropertyNode *node)
{
    if (!node)
        return;

    if (node->name)
        fl_cstring_free(node->name);

    zenit_node_free(node->value);

    fl_free(node);
}
