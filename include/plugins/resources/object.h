#ifndef TICO_PLUGIN_RESOURCES_OBJECT_H
#define TICO_PLUGIN_RESOURCES_OBJECT_H

#include "../../definitions.h"

TIC_API tc_ResourcePlugin tico_plugin_resource_object();
TIC_API tc_Resource tico_plugin_resource_object_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json);
TIC_API cJSON* tico_plugin_resource_object_store(tc_ResourceManager *manager, tc_Resource *res);

#endif

#if defined(TICO_PLUGIN_RESOURCES_IMPLEMENTATION)

tc_ResourcePlugin tico_plugin_resource_object() {
  // tc_ResourcePlugin plugin = {0};
  // plugin.loader = (PluginResourceLoader)tico_plugin_resource_object_loader;
  // plugin.store = (PluginResourceStore)tico_plugin_resource_object_store;

  // return plugin;
  return tico_plugin_resource_json();
}

tc_Resource tico_plugin_resource_object_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json) {
	// if (!res) return NULL;

	// cJSON *root = json;
	// tc_Plugin *plugin = tico_plugin_get("lua");
	cJSON* json_clone = tico_json_clone(json);

	if (manager->lua) {
		tc_Lua *lua = manager->lua;
		tico_lua_json_new_object(lua->L, json_clone);
	}

	res->status |= TIC_RESOURCE_LOADED;
	res->data = json_clone;

	return *res;
}

cJSON* tico_plugin_resource_object_store(tc_ResourceManager *manager, tc_Resource *res) {}

#endif