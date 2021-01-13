#include "tico.h"

#define TICO_PLUGIN_IMPLEMENTATION
#include "plugins/editor.h"
#include "plugins/resources.h"
#include "plugins/tclua.h"

tc_Plugin tico_plugin_create() {
	tc_Plugin plugin = {0};
	memset(&plugin, 0, sizeof(tc_Plugin));
	return plugin;
}

tc_Plugin tico_plugin_editor() {
	// tico_plugin_init(plugin);
	tc_Plugin plugin = tico_plugin_create();
	plugin.data = TIC_MALLOC(sizeof(tc_Editor));
	*(tc_Editor*)plugin.data = (tc_Editor){0};
	plugin.enable = (PluginLoad)tico_plugin_editor_load;
	plugin.update = (PluginUpdate)tico_plugin_editor_update;
	plugin.draw = (PluginDraw)tico_plugin_editor_draw;

	return plugin;
}

tc_Plugin tico_plugin_lua() {
	tc_Plugin plugin = tico_plugin_create();
	plugin.data = TIC_MALLOC(sizeof(tc_Lua));
	*(tc_Lua*)plugin.data = (tc_Lua){0};
	plugin.enable = (PluginEnable)tico_plugin_lua_enable;
	plugin.load = (PluginLoad)tico_plugin_lua_load;
	plugin.update = (PluginUpdate)tico_plugin_lua_update;
	plugin.draw = (PluginDraw)tico_plugin_lua_draw;

	return plugin;
}

tc_Plugin tico_plugin_resource() {
	tc_Plugin plugin = tico_plugin_create();
	plugin.data = TIC_MALLOC(sizeof(tc_ResourceManager));
	*(tc_ResourceManager*)plugin.data = (tc_ResourceManager){0};
	plugin.enable = (PluginLoad)tico_plugin_resources_init;

	return plugin;
}

int tico_plugin_destroy(tc_Plugin *plugin) {
	ASSERT(plugin != NULL);
	if (plugin->terminate) plugin->terminate(plugin->data);
	return 1;
}

void tico_plugin_enable(tc_Plugin *plugin) {
	ASSERT(plugin != NULL);
	if (plugin->enable) plugin->enable(plugin->data);
}
void tico_plugin_disable(tc_Plugin *plugin) {
	ASSERT(plugin != NULL);
	if (plugin->disable) plugin->disable(plugin->data);
}

void tico_plugin_load(tc_Plugin *plugin) {
	ASSERT(plugin != NULL);
	if (plugin->load) plugin->load(plugin->data);
}
void tico_plugin_update(tc_Plugin *plugin) {
	ASSERT(plugin != NULL);
	if (plugin->update) plugin->update(plugin->data);
}
void tico_plugin_begin_draw(tc_Plugin *plugin) {
	ASSERT(plugin != NULL);
	if (plugin->pre_draw) plugin->pre_draw(plugin->data);
}
void tico_plugin_draw(tc_Plugin *plugin) {
	ASSERT(plugin != NULL);
	if (plugin->draw) plugin->draw(plugin->data);
}
void tico_plugin_end_draw(tc_Plugin *plugin) {
	ASSERT(plugin != NULL);
	if (plugin->post_draw) plugin->post_draw(plugin->data);
}


/**********************
 * Plugin System
 **********************/

void INTERNAL_API(tico_plugin_add_plugin, tc_PluginModule *module, const char *name, tc_Plugin plugin) {
	ASSERT(module != NULL);
	ASSERT(name != NULL);
	map_set(&module->plugins, name, plugin);
}
void INTERNAL_API(tico_plugin_enable_plugin, tc_PluginModule *module, const char *name) {
	ASSERT(module != NULL);
	ASSERT(name != NULL);
	if (tico_plugin_is_active(name)) return;
	// list_push(&module->active_plugins, (char*)name);
	vec_push(&module->active_plugins, (char*)name);
	tc_Plugin *plugin = map_get(&module->plugins, name);
	// if (plugin && plugin->load) plugin->load(plugin->data);
	if (plugin && plugin->enable) plugin->enable(plugin->data);

	LOG("Plugin: %s enabled", name);
}
void INTERNAL_API(tico_plugin_disable_plugin, tc_PluginModule *module, const char *name) {
	ASSERT(module != NULL);
	ASSERT(name != NULL);
	int i = 0;
	int found = 0;
	char *key;
  vec_foreach(&module->active_plugins, key, i) {
		if (!strcmp(name, key)) {
			found = 1;
			break;
		}
		i++;
	}

	LOG("Plugin: %s disabled", name);

	// if (found) list_remove(&module->active_plugins, i);
}
tc_bool INTERNAL_API(tico_plugin_is_active, tc_PluginModule *module, const char *name) {
	ASSERT(module != NULL);
	ASSERT(name != NULL);

	// list_iter_t iter = list_iter(&module->active_plugins);
	char *key;
  int i;
  vec_foreach(&module->active_plugins, key, i) {
    if (!strcmp(name, key)) return 1;
  }

	return 0;
}
tc_Plugin* INTERNAL_API(tico_plugin_get, tc_PluginModule *module, const char *name) {
	ASSERT(module != NULL);
	ASSERT(name != NULL);

	tc_Plugin *plugin = map_get(&module->plugins, name);

	return plugin;
}

int INTERNAL_API(tico_plugin_module_init, tc_PluginModule *module) {
	ASSERT(module != NULL);
	memset(module, 0, sizeof(*module));

	map_init(&module->plugins);
	// list_init(&module->active_plugins);
	vec_init(&module->active_plugins);

	tc_Plugin editor = tico_plugin_editor();
	tc_Plugin lua = tico_plugin_lua();
	tc_Plugin resources = tico_plugin_resource();

	tico_plugin_add_plugin_internal(module, "editor", editor);
	tico_plugin_add_plugin_internal(module, "lua", lua);
	tico_plugin_add_plugin_internal(module, "resources", resources);

	return 1;
}
void INTERNAL_API(tico_plugin_module_terminate, tc_PluginModule *module) {
	map_iter_t iter = map_iter(&module->plugins);
	const char *key;
	while ((key = map_next(&module->plugins, &iter))) {
		tc_Plugin *plugin = map_get(&module->plugins, key);
		tico_plugin_destroy(plugin);
	}
}

void INTERNAL_API(tico_plugin_module_enable, tc_PluginModule *module) {
	char *key;
  int i;
  vec_foreach(&module->active_plugins, key, i) {
		// char **key = iter.data;
		tc_Plugin *plugin = map_get(&module->plugins, key);
		if (!plugin) continue;
		tico_plugin_enable(plugin);
	}
}
void INTERNAL_API(tico_plugin_module_disable, tc_PluginModule *module) {
	char *key;
  int i;
  vec_foreach(&module->active_plugins, key, i) {
		// char **key = iter.data;
		tc_Plugin *plugin = map_get(&module->plugins, key);
		if (!plugin) continue;
		tico_plugin_disable(plugin);
	}
}


void INTERNAL_API(tico_plugin_module_load, tc_PluginModule *module) {
	ASSERT(module != NULL);
	char *key;
  int i;
  vec_foreach(&module->active_plugins, key, i) {
		// char **key = iter.data;
		tc_Plugin *plugin = map_get(&module->plugins, key);
		if (!plugin) continue;
		tico_plugin_load(plugin);
	}
}

void INTERNAL_API(tico_plugin_module_update, tc_PluginModule *module) {
	char *key;
  int i;
  vec_foreach(&module->active_plugins, key, i) {
		// char **key = iter.data;
		tc_Plugin *plugin = map_get(&module->plugins, key);
		if (!plugin) continue;
		tico_plugin_update(plugin);
	}
}
void INTERNAL_API(tico_plugin_module_begin_draw, tc_PluginModule *module) {
	char *key;
  int i;
  vec_foreach(&module->active_plugins, key, i) {
		// char **key = iter.data;
		tc_Plugin *plugin = map_get(&module->plugins, key);
		if (!plugin) continue;
		tico_plugin_begin_draw(plugin);
	}
}
void INTERNAL_API(tico_plugin_module_draw, tc_PluginModule *module) {
	char *key;
  int i;
  vec_foreach(&module->active_plugins, key, i) {
		// char **key = iter.data;
		tc_Plugin *plugin = map_get(&module->plugins, key);
		if (!plugin) continue;
		tico_plugin_draw(plugin);
	}
}
void INTERNAL_API(tico_plugin_module_end_draw, tc_PluginModule *module) {
	char *key;
  int i;
  vec_foreach(&module->active_plugins, key, i) {
		// char **key = iter.data;
		tc_Plugin *plugin = map_get(&module->plugins, key);
		if (!plugin) continue;
		tico_plugin_end_draw(plugin);
	}
}