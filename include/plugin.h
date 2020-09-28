#ifndef TICO_PLUGIN_H
#define TICO_PLUGIN_H

#include "tico.h"

// #define tico_plugin_call()

typedef int(*PluginEnable)(void*);
typedef int(*PluginDisable)(void*);
typedef int(*PluginLoad)(void*);
typedef int(*PluginUpdate)(void*);
typedef int(*PluginPreDraw)(void*);
typedef int(*PluginDraw)(void*);
typedef int(*PluginPostDraw)(void*);
typedef int(*PluginTerminate)(void*);
// typedef int(*PluginDraw)(void*);

typedef struct tc_Plugin {
	void *data;

	PluginEnable enable;
	PluginDisable disable;

	PluginLoad load;
	PluginUpdate update;
	PluginPreDraw pre_draw;
	PluginDraw draw;
	PluginPostDraw post_draw;
	PluginTerminate terminate;

} tc_Plugin;

typedef map_t(tc_Plugin) map_plugin_t;

typedef struct tc_PluginModule {
	map_plugin_t plugins;
	list_str_t active_plugins;
} tc_PluginModule;

TIC_API tc_Plugin tico_plugin_create();
TIC_API tc_Plugin tico_plugin_editor();
TIC_API tc_Plugin tico_plugin_lua();
TIC_API tc_Plugin tico_plugin_resource();
TIC_API int tico_plugin_destroy(tc_Plugin *plugin);

TIC_API void tico_plugin_enable(tc_Plugin *plugin);
TIC_API void tico_plugin_disable(tc_Plugin *plugin);

TIC_API void tico_plugin_update(tc_Plugin *plugin);
TIC_API void tico_plugin_begin_draw(tc_Plugin *plugin);
TIC_API void tico_plugin_draw(tc_Plugin *plugin);
TIC_API void tico_plugin_end_draw(tc_Plugin *plugin);

TIC_API void INTERNAL_API(tico_plugin_add_plugin, tc_PluginModule *module, const char *name, tc_Plugin plugin);
TIC_API void INTERNAL_API(tico_plugin_enable_plugin, tc_PluginModule *module, const char *name);
TIC_API void INTERNAL_API(tico_plugin_disable_plugin, tc_PluginModule *module, const char *name);
TIC_API tc_bool INTERNAL_API(tico_plugin_is_active, tc_PluginModule *module, const char *name);
TIC_API tc_Plugin* INTERNAL_API(tico_plugin_get, tc_PluginModule *module, const char *name);

TIC_API int INTERNAL_API(tico_plugin_module_init, tc_PluginModule *module);
TIC_API void INTERNAL_API(tico_plugin_module_terminate, tc_PluginModule *module);

TIC_API void INTERNAL_API(tico_plugin_module_enable, tc_PluginModule *module);
TIC_API void INTERNAL_API(tico_plugin_module_disable, tc_PluginModule *module);

TIC_API void INTERNAL_API(tico_plugin_module_load, tc_PluginModule *module);
TIC_API void INTERNAL_API(tico_plugin_module_update, tc_PluginModule *module);
TIC_API void INTERNAL_API(tico_plugin_module_begin_draw, tc_PluginModule *module);
TIC_API void INTERNAL_API(tico_plugin_module_draw, tc_PluginModule *module);
TIC_API void INTERNAL_API(tico_plugin_module_end_draw, tc_PluginModule *module);

/*================*
 *      Lua       *
 *================*/

#include "plugins/tclua.h"

/*================*
 *     Editor     *
 *================*/

#include "plugins/editor.h"

/*=======================*
 *   Resource Manager    *
 *=======================*/

#include "plugins/resources.h"

#endif // TICO_PLUGIN_H