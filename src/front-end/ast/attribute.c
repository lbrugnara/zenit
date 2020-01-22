#include "attribute.h"

struct ZenitAttributeNode* zenit_node_attribute_new(struct ZenitSourceLocation location, char *name)
{
    struct ZenitAttributeNode *attribute = fl_malloc(sizeof(struct ZenitAttributeNode));
    attribute->base.nodekind = ZENIT_NODE_ATTRIBUTE;
    attribute->base.location = location;
    attribute->name = name;
    attribute->properties = zenit_property_node_map_new();

    return attribute;
}

char* zenit_node_attribute_uid(struct ZenitAttributeNode *attribute)
{
    if (!attribute)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_attr_%s", attribute->base.location.line, attribute->base.location.col, attribute->name);
}

char* zenit_node_attribute_dump(struct ZenitAttributeNode *attribute, char *output)
{
    fl_cstring_vappend(&output, "(attr %s", attribute->name);

    struct ZenitPropertyNode **properties = zenit_property_node_map_values(&attribute->properties);
    
    size_t length = fl_array_length(properties);
    if (length > 0)
    {
        fl_cstring_append(&output, " ");
        for (size_t i=0; i < length; i++)
        {
            output = zenit_node_property_dump(properties[i], output);
            if (i != length - 1)
                fl_cstring_append(&output, " ");
        }
    }

    fl_array_free(properties);

    fl_cstring_append(&output, ")");

    return output;
}

void zenit_node_attribute_free(struct ZenitAttributeNode *attribute_node)
{
    if (!attribute_node)
        return;

    if (attribute_node->name)
        fl_cstring_free(attribute_node->name);

    zenit_property_node_map_free(&attribute_node->properties);

    fl_free(attribute_node);
}
