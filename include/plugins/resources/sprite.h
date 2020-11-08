#ifndef TICO_PLUGIN_RESOURCE_SPRITE_H
#define TICO_PLUGIN_RESOURCE_SPRITE_H

TIC_API tc_ResourcePlugin tico_plugin_resource_sprite();
TIC_API tc_Resource tico_plugin_resource_sprite_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json);
TIC_API cJSON* tico_plugin_resource_sprite_store(tc_ResourceManager *manager, tc_Resource *res);

#endif

#if defined(TICO_PLUGIN_RESOURCES_IMPLEMENTATION)

// #include "../lua/json.h"

tc_ResourcePlugin tico_plugin_resource_sprite() {
	return tico_plugin_resource_json();
}

#endif