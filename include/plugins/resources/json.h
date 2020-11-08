#ifndef TICO_PLUGIN_RESOURCE_JSON_H
#define TICO_PLUGIN_RESOURCE_JSON_H

TIC_API tc_ResourcePlugin tico_plugin_resource_json();
TIC_API tc_Resource tico_plugin_resource_json_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json);
TIC_API cJSON* tico_plugin_resource_json_store(tc_ResourceManager *manager, tc_Resource *res);

#endif

#if defined(TICO_PLUGIN_RESOURCES_IMPLEMENTATION)

#include "../lua/json.h"

tc_ResourcePlugin tico_plugin_resource_json() {
	return tico_plugin_resource_init(tico_plugin_resource_json_loader, tico_plugin_resource_json_store);
}

tc_Resource tico_plugin_resource_json_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json) {
	ASSERT(json != NULL);
	if (manager->lua) {

		tico_lua_json_new_object(manager->lua->L, json);
	}

	res->status |= TIC_RESOURCE_LOADED;
	res->data = json;

	return *res;
}

cJSON* tico_plugin_resource_json_store(tc_ResourceManager *manager, tc_Resource *resource) {
	if (!resource) return NULL;


	cJSON *root = resource->data;
	tico_json_save(resource->path, root);
	
	return root;
}

#endif