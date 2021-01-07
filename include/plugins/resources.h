#ifndef TICO_RESOURCES_H
#define TICO_RESOURCES_H

#include "../tico.h"
#include "tclua.h"

#define RESOURCE_PLUGIN(name) \
TIC_API tc_ResourcePlugin tico_plugin_resource_##name(); \
TIC_API tc_Resource tico_plugin_resource_##name##_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json); \
TIC_API cJSON* tico_plugin_resource_##name##_store(tc_ResourceManager *manager, tc_Resource *res);

#define MAX_RESOURCES_DEPS 32

typedef struct tc_Resource tc_Resource;
typedef struct tc_ResourceDep tc_ResourceDep;
typedef struct tc_ResourcePlugin tc_ResourcePlugin;

struct tc_ResourceDep {
  char name[256];
  char res_key[256];
  tc_Resource *res;
  // tc_ResourceDep *next;
};

typedef map_t(tc_ResourceDep) map_deps_t;
typedef list_t(tc_ResourceDep) list_resdep_t;

#define RES_UUID_SIZE 8

enum {
  TIC_RESOURCE_ERROR = (1 << 0),
  TIC_RESOURCE_LOADED = (1 << 1),
  TIC_RESOURCE_MISSING_DEP = (1 << 2),
  TIC_RESOURCE_CHANGED = (1 << 3),
};

// vec_t(tc_ResourceDep)

struct tc_Resource {
  int status;
  // int deps_count;
  // int id;
  int changed;
  // int lua;
  char type[64];
  char uuid[RES_UUID_SIZE+1];
  char name[256];
  char path[256];
  void *data;
//   lua_State *L;
// #ifdef WREN_LANG
//   WrenVM *vm;
// #endif
  // list_resdep_t deps;
  // map_deps_t dep;
};

typedef struct tc_ResourceManager tc_ResourceManager;

typedef tc_Resource(*PluginResourceLoader)(tc_ResourceManager*, tc_Resource*, cJSON*);
typedef cJSON*(*PluginResourceStore)(tc_ResourceManager*, tc_Resource*);

struct tc_ResourcePlugin {
	PluginResourceLoader loader;
	PluginResourceStore store;
};

TIC_API tc_ResourcePlugin tico_plugin_resource_init(PluginResourceLoader loader, PluginResourceStore store);

/*************************
 * Resources Plugin
 *************************/

#include "resources/json.h"

#include "resources/image.h"
#include "resources/sprite.h"
#include "resources/tileset.h"
#include "resources/tilemap.h"
#include "resources/object.h"

typedef map_t(tc_Resource) map_resource_t;
typedef map_t(tc_Resource*) map_resref_t;
typedef map_t(tc_ResourcePlugin) map_resplugin_t;


struct tc_ResourceManager {
  char path[256];
  map_resource_t resources;
  map_resplugin_t plugins;
  // map_resref_t missing;
  map_str_t missing;
  map_str_t keys;
  tc_Lua *lua;
  tc_Field *fields;
};

TIC_API void tico_resource_init(tc_Resource *resource);
TIC_API int tico_resource_change_lua(tc_Resource *resource);

TIC_API tc_bool tico_plugin_resources_init(tc_ResourceManager *manager);
TIC_API int tico_plugin_resources_default_loaders(tc_ResourceManager *manager);
TIC_API tc_bool tico_plugin_resources_load(tc_ResourceManager *manager, const char *type, const char *name, const char *path);
TIC_API tc_Resource* tico_plugin_resources_from_json(tc_ResourceManager *manager, cJSON const *json);
// TIC_API void tico_plugin_resources_store(tc_ResourceManager *manager);
TIC_API tc_bool tico_plugin_resources_destroy(tc_ResourceManager *manager);

TIC_API int tico_plugin_resources_check_json_deps(tc_ResourceManager *manager, cJSON *json);
TIC_API void tico_plugin_resources_update_missing(tc_ResourceManager *manager);
TIC_API tc_ResourceDep tico_plugin_resources_new_dep(const char *name, tc_Resource *res);
TIC_API void tico_plugin_resources_push_dep(tc_Resource *res, const char *name, const char *resource_name);
TIC_API void tico_plugin_resources_change_dep(tc_ResourceManager *manager, const char *name, const char *res_name);
TIC_API void tico_plugin_resources_new_type(tc_ResourceManager *manager, const char *type);

TIC_API void tico_plugin_resources_update_resource(tc_Resource *resource);

TIC_API tc_bool tico_plugin_resources_add(tc_ResourceManager *manager, const char *type, const char *name, tc_Resource *resource);

TIC_API int PUBLIC_API(tico_plugin_resources_store, tc_Resource *resource);
TIC_API int INTERNAL_API(tico_plugin_resources_store, tc_ResourceManager *manager, tc_Resource *resource);

TIC_API tc_Resource* PUBLIC_API(tico_plugin_resources_get_by_field, tc_Field *field);
TIC_API tc_Resource* PUBLIC_API(tico_plugin_resources_get_by_key, const char *key);
TIC_API tc_Resource* PUBLIC_API(tico_plugin_resources_get_by_uuid, const char *uuid);
TIC_API tc_Resource* PUBLIC_API(tico_plugin_resources_get, const char *type, const char *name);
TIC_API tc_Resource* INTERNAL_API(tico_plugin_resources_get_by_field, tc_ResourceManager *manager, tc_Field *field);
TIC_API tc_Resource* INTERNAL_API(tico_plugin_resources_get_by_key, tc_ResourceManager *manager, const char *key);
TIC_API tc_Resource* INTERNAL_API(tico_plugin_resources_get_by_uuid, tc_ResourceManager *manager, const char *uuid);
TIC_API tc_Resource* INTERNAL_API(tico_plugin_resources_get, tc_ResourceManager *manager, const char *type, const char *name);
#endif // TICO_RESOURCES_H

#if defined(TICO_PLUGIN_IMPLEMENTATION)

/*********************
 * Resource Plugins
 *********************/

tc_ResourcePlugin tico_plugin_resource_init(PluginResourceLoader loader, PluginResourceStore store) {
	tc_ResourcePlugin plugin = {0};
	plugin.loader = loader;
	plugin.store = store;

	return plugin;
}

#define TICO_PLUGIN_RESOURCES_IMPLEMENTATION
#include "lua/resources.h"
#include "resources/json.h"

#include "resources/image.h"
#include "resources/sprite.h"
#include "resources/tileset.h"
#include "resources/tilemap.h"
#include "resources/object.h"
// #define RES_KEY(type, name) #type"//"#name

void tico_resource_init(tc_Resource *resource) {
	ASSERT(resource != NULL);
	memset(resource, 0, sizeof(*resource));
	// map_init(&resource->dep);
}

int tico_resource_change_lua(tc_Resource *resource) {
	ASSERT(resource != NULL);
	tc_Plugin *plugin = tico_plugin_get("lua");
	if (!plugin) return 0;

	tc_Lua *lua = plugin->data;
	int top = lua_gettop(lua->L);
	lua_getglobal(lua->L, "__resources");
	lua_pushvalue(lua->L, top);
	lua_setfield(lua->L, -2, resource->uuid);

	return 1;
}

tc_Resource tico_plugin_resources_create_resource(const char *type, const char *name) {
	tc_Resource res = {0};
	memset(&res, 0, sizeof(res));
	// tico_utils_generate_uuid(res.uuid, 8);
	strcpy(res.type, type);
	strcpy(res.name, name);
	// map_init(&res.dep);

	return res;
}

tc_Resource tico_resource_from_json(cJSON const *json) {
	ASSERT(json != NULL);
	tc_Resource resource = {0};
	cJSON *header = tico_json_get_object(json, "__header__", 0);
	if (!header) header = json;

	const char *type = tico_json_get_string(header, "type", 0);
	const char *name = tico_json_get_string(header, "name", 0);
	const char *path = tico_json_get_string(header, "path", 0);

	if (type) strcpy(resource.type, type);
	if (name) strcpy(resource.name, name);
	if (path) strcpy(resource.path, path);

	return resource;
}

int tico_plugin_resources_init(tc_ResourceManager *manager) {
	manager->lua = NULL;

	tico_plugin_resources_default_loaders(manager);

	tc_Plugin *plugin = tico_plugin_get("lua");
	if (plugin) {
		tc_Lua *lua = plugin->data;
		manager->lua = lua;
		lua_newtable(lua->L);
		lua_setglobal(lua->L, "__resources");
		tico_plugin_lua_add_lib("Resource", luaopen_resource);
		tico_plugin_lua_add_lib("resources", luaopen_resources);
	}
	if (!manager->fields) manager->fields = tico_field_create_object(NULL);
		// TRACELOG("ok %p", lua->L);
  manager->path[0] = '\0';
  map_init(&manager->keys);
  map_init(&manager->resources);
  map_init(&manager->missing);
	// strcpy(manager->path, path ? path : "resources.json");
	strcpy(manager->path, PROJECT_FILE);

	cJSON *json = tico_json_open(manager->path);
	if (!json) return 0;

	cJSON *resources = tico_json_get_item(json, "resources", 0);
	if (!resources) {
		tico_json_delete(json);
		return 0;
	}


	cJSON *el = NULL;
	cJSON *resource = NULL;
	cJSON_ArrayForEach(el, resources) {
		if (el->type == cJSON_Object) {
			// tc_Resource res = tico_resource_from_json(el);
			tico_plugin_resources_from_json(manager, el);
		} else if (el->type == cJSON_String) {
			cJSON *res_data = tico_json_open(el->valuestring);
			if (res_data) {
				tc_Resource *res = tico_plugin_resources_from_json(manager, res_data);
				if (res) strcpy(res->path, el->valuestring);
			}
		}
		// if (el->string) {
  //     tc_ResourcePlugin *plugin = map_get(&manager->plugins, el->string);
  //     if (!plugin && !plugin->loader) continue;

		// 	cJSON_ArrayForEach(resource, el) {
		// 		const char *name = resource->string;
		// 		// TRACELOG("%s %s", el->string, name);
		// 		tico_plugin_resources_load(manager, el->string, name, tico_json_get_string(resource, "path", 0));
		// 	}
		// }
	}

	tico_json_delete(json);

	return 1;
}

int tico_plugin_resources_default_loaders(tc_ResourceManager *manager) {
	manager->fields = tico_field_create_object(NULL);
  map_init(&manager->plugins);
  map_set(&manager->plugins, "image", tico_plugin_resource_image());
  map_set(&manager->plugins, "tileset", tico_plugin_resource_tileset());
  map_set(&manager->plugins, "tilemap", tico_plugin_resource_tilemap());
  map_set(&manager->plugins, "object", tico_plugin_resource_object());
  map_set(&manager->plugins, "sprite", tico_plugin_resource_sprite());

  const char *types[5] = {"image", "tileset", "tilemap", "object", "sprite"};
  for (int i = 0; i < 5; i++) {
  	tc_Field *field = tico_field_create_object(NULL);
  	tico_field_set(manager->fields, types[i], field);
  }
  // tico_field_add
  // map_set(&manager->plugins, "sprite", tico_plugin_resource_sprite());
  TRACELOG("default loaders");
}

enum {
	TIC_LOADER_IMAGE = 1,
	TIC_LOADER_TILESET,
	TIC_LOADER_TILEMAP
};

// int tico_plugin_resources_load(tc_ResourceManager *manager, const char *type, const char *name, const char *path) {
// 	if (!tico_filesystem_file_exists((const unsigned char*)path)) return 0;

// 	cJSON *json = NULL;
// 	if (tico_json_is_valid(path)) json = tico_json_open(path);
// 	// if (!strcmp(type, "sprite")) TRACELOG("%s %p", type, json);

// 	tc_ResourcePlugin *plugin = map_get(&manager->plugins, type);

// 	if (!plugin) {
// 		if (json) tico_json_delete(json);
// 		return 0;
// 	}

// 	tc_Resource res = tico_plugin_resources_create_resource(type, name);
// 	if (manager->lua) {
// 		res.lua = 1;
// 		res.L = manager->lua->L;
// 	}
// 	lua_settop(res.L, 0);
// 	strcpy(res.path, path);

// 	if (json) {
// 		int sz = strlen(type) + strlen(name) + 2;
// 		char nm[sz];
// 		sprintf(nm, "%s//%s", type, name);


// 		cJSON *deps = tico_json_get_object(json, "dependencies", 0);
// 		cJSON *dep = NULL;
// 		if (deps) {
// 			if (!tico_plugin_resources_check_json_deps(manager, json)) {
// 				map_set(&manager->missing, nm, (char*)path);
// 				tico_json_delete(json);
// 				return 0;
// 			}
// 			cJSON_ArrayForEach(dep, deps) {
// 				// tc_Resource *resdep = map_get(&manager->resources, dep->valuestring);
// 				tc_Resource *resdep = tico_plugin_resources_get_by_key_internal(manager, dep->valuestring);
// 				// if (!resdep) res.status |= TIC_RESOURCE_MISSING_DEP;
// 				tc_ResourceDep rdep = tico_plugin_resources_new_dep(dep->string, resdep);
// 				strcpy(rdep.res_key, dep->valuestring);
// 				map_set(&res.dep, dep->string, rdep);
// 				// TRACELOG("%s", dep->string);
// 			}

// 			// if (res.status & TIC_RESOURCE_MISSING_DEP) {
// 			// 	map_set(&manager->resources, nm, res);
// 			// 	map_set(&manager->missing, nm, path);
// 			// 	tico_json_delete(json);
// 			// 	return 0;
// 			// }
// 			char **dp = map_get(&manager->missing, nm);
// 			if (dp) map_remove(&manager->missing, nm);
// 		}

// 		// exit(1);
// 	}

// 	// TRACELOG("%s", type);
// 	plugin->loader(manager, &res, json);
// 	if (res.status & TIC_RESOURCE_LOADED) tico_plugin_resources_add(manager, type, name, &res);

// 	tico_plugin_resources_update_missing(manager);

// 	return 1;
// }

tc_Resource* tico_plugin_resources_from_json(tc_ResourceManager *manager, cJSON const *json) {
	ASSERT(manager != NULL);
	if (!json) return NULL;

	tc_Resource resource = tico_resource_from_json(json);
	if (manager->lua) {
		lua_settop(manager->lua->L, 0);
	}


	tc_ResourcePlugin *plugin = map_get(&manager->plugins, resource.type);
	if (!plugin) return NULL;
	if (!plugin->loader) return NULL;

	plugin->loader(manager, &resource, json);
	if (resource.status & TIC_RESOURCE_LOADED) tico_plugin_resources_add(manager, NULL, NULL, &resource);


	tc_Resource *res = map_get(&manager->resources, resource.uuid);

	return res;
}

// void tico_plugin_resources_store(tc_ResourceManager *manager) {
// 	const char *key;
// 	map_iter_t iter = map_iter(&manager->resources);
// 	cJSON *res_json = tico_json_create();
// 	while((key = map_next(&manager->resources, &iter))) {
// 		tc_Resource *res = map_get(&manager->resources, key);
// 		tc_ResourcePlugin *plugin = map_get(&manager->plugins, res->type);
// 		cJSON *jdata = NULL;
// 		if (plugin->store) jdata = plugin->store(manager, res);
// 		if (jdata) {
// 			cJSON *deps = tico_json_get_object(jdata, "dependencies");
// 			if (deps) tico_json_delete(deps);
// 			deps = tico_json_set_object(jdata, "dependencies", NULL);
// 			const char *dep_name;
// 			map_iter_t diter = map_iter(&res->dep);
// 			while ((dep_name = map_next(&res->dep, &diter))) {
// 				tc_ResourceDep *rdp = map_get(&res->dep, dep_name);
// 				tico_json_set_string(deps, dep_name, rdp->res_key);
// 			}
// 			tico_json_save(res->path, jdata);
// 			tico_json_delete(jdata);
// 		}
// 		// TRACELOG("%d %s", res->type & TIC_RESOURCE_IMAGE, key);
// 		cJSON *category = tico_json_get_object(res_json, res->type);
// 		if (!category) category = tico_json_set_object(res_json, res->type, NULL);

// 		cJSON *jres = tico_json_create();
// 		tico_json_set_string(jres, "path", res->path);
// 		tico_json_set_boolean(jres, "lua", res->lua);
// 		tico_json_set_object(category, res->name, jres);
// 	}

// 	tico_json_save(manager->path, res_json);
// 	tico_json_delete(res_json);
// }

tc_bool tico_plugin_resources_add(tc_ResourceManager *manager, const char *type, const char *name, tc_Resource *resource) {
	if (~resource->status & TIC_RESOURCE_LOADED) return 0;
	if (type) strcpy(resource->type, type);
	if (name) strcpy(resource->name, name);

	int type_len = strlen(resource->type);
	int name_len = strlen(resource->name);
  int size = name_len + type_len + 3;
  char key[size];
  sprintf(key, "%s//%s", resource->type, resource->name);
  char **uuid = map_get(&manager->keys, key);

	if (uuid) return 0;


	tico_utils_generate_uuid(resource->uuid, 8);
	while(map_get(&manager->resources, resource->uuid)) {
		tico_utils_generate_uuid(resource->uuid, 8);
	}

	map_set(&manager->resources, resource->uuid, *resource);
	if (manager->lua) {
		tico_resource_change_lua(resource);
		// TRACELOG("%s %s %s", resource->type, resource->name, resource->uuid);
	}

	tc_Resource* res = map_get(&manager->resources, resource->uuid);
	map_set(&manager->keys, key, res->uuid);
	tc_Field *type_field = tico_field_get(manager->fields, resource->type);
	if (type_field) {
		tc_Field *field = tico_field_create_string(resource->uuid);
		// tico_field_add(type_field, resource->name, field);
		tico_field_set(type_field, resource->name, field);
		tc_Field *tst = tico_field_get(type_field, resource->name);
		// TRACELOG("%p", tst);
	}


	return 1;
}

int PUBLIC_API(tico_plugin_resources_store, tc_Resource *resource) {
	tc_Plugin *plugin = tico_plugin_get("resources");
	if (!plugin) return 0;
	tc_ResourceManager *manager = plugin->data;
	return INTERNAL_API(tico_plugin_resources_store, manager, resource);
}
int INTERNAL_API(tico_plugin_resources_store, tc_ResourceManager *manager, tc_Resource *resource) {
	ASSERT(manager != NULL);
	ASSERT(resource != NULL);

	tc_ResourcePlugin *plugin = map_get(&manager->plugins, resource->type);
	if (!plugin) return 0;

	plugin->store(manager, resource);

	return 1;
}

tc_Resource* PUBLIC_API(tico_plugin_resources_get, const char *type, const char *name) {
	ASSERT(type != NULL);
	ASSERT(name != NULL);
	tc_Plugin *plugin = tico_plugin_get("resources");
	if (!plugin) return NULL;
	tc_ResourceManager *manager = plugin->data;
	return INTERNAL_API(tico_plugin_resources_get, manager, type, name);
}

tc_Resource* INTERNAL_API(tico_plugin_resources_get, tc_ResourceManager *manager, const char *type, const char *name) {
	tc_Resource *res = NULL;
  int size = strlen(type) + strlen(name) + 2;
  char key[size];
  sprintf(key, "%s//%s", type, name);
  res = tico_plugin_resources_get_by_key_internal(manager, key);

	return res;
}

tc_Resource* PUBLIC_API(tico_plugin_resources_get_by_uuid, const char *uuid) {
	tc_Plugin *plugin = tico_plugin_get("resources");
	if (!plugin) return NULL;
	tc_ResourceManager *manager = plugin->data;
	return INTERNAL_API(tico_plugin_resources_get_by_uuid, manager, uuid);
}

tc_Resource* INTERNAL_API(tico_plugin_resources_get_by_uuid, tc_ResourceManager *manager, const char *uuid) {
	ASSERT(manager != NULL);
	tc_Resource* res = map_get(&manager->resources, uuid);
	if (!res) return NULL;

	tc_Plugin *plugin = tico_plugin_get("lua");

	if (plugin) {
		tc_Lua *lua = plugin->data;
		lua_getglobal(lua->L, "__resources");
		lua_getfield(lua->L, -1, uuid);
		if (!lua_isnil(lua->L, -1)) lua_pushvalue(lua->L, -1);
		else lua_pushnil(lua->L);
	}

	return res;
}

tc_Resource* PUBLIC_API(tico_plugin_resources_get_by_field, tc_Field *field) {
	tc_Plugin *plugin = tico_plugin_get("resources");
	if (!plugin) return NULL;
	tc_ResourceManager *manager = plugin->data;
	return INTERNAL_API(tico_plugin_resources_get_by_field, manager, field);
}

tc_Resource* INTERNAL_API(tico_plugin_resources_get_by_field, tc_ResourceManager *manager, tc_Field *field) {
	ASSERT(manager != NULL);

	tc_Resource *res = tico_plugin_resources_get_by_key_internal(manager, field->string);
	return res;
}

tc_Resource* PUBLIC_API(tico_plugin_resources_get_by_key, const char *key) {
	tc_Plugin *plugin = tico_plugin_get("resources");
	if (!plugin) return NULL;
	tc_ResourceManager *manager = plugin->data;
	return INTERNAL_API(tico_plugin_resources_get_by_key, manager, key);
}

tc_Resource* INTERNAL_API(tico_plugin_resources_get_by_key, tc_ResourceManager *manager, const char *key) {
	tc_Resource *res = NULL;
	char **uuid = map_get(&manager->keys, key);
	if (!uuid) return NULL;

	// TRACELOG("%s", *uuid);

	res = INTERNAL_API(tico_plugin_resources_get_by_uuid, manager, *uuid);

	return res;
}

int tico_plugin_resources_check_json_deps(tc_ResourceManager *manager, cJSON *json) {
	if (!json) return 0;
	cJSON *deps = tico_json_get_object(json, "dependencies", 0);
	if (!deps) return 0;

	cJSON *dep = NULL;
	cJSON_ArrayForEach(dep, deps) {
		tc_Resource *res = tico_plugin_resources_get_by_key_internal(manager, dep->valuestring);
		if (!res) return 0; 
	}

	return 1;
}

// void tico_plugin_resources_update_missing(tc_ResourceManager *manager) {
// 	map_iter_t iter = map_iter(&manager->missing);
// 	const char *key;
// 	while((key = map_next(&manager->missing, &iter))) {
// 		char **path = map_get(&manager->missing, key);
// 		if (!path) continue;
// 		char *name = strstr(key, "//");
// 		int type_sz = name - key;
// 		char type[type_sz];
// 		memcpy(type, key, type_sz);
// 		name += 2;
// 		// TRACELOG("%s %s %s", type, name, *path);
// 		tico_plugin_resources_load(manager, type, name, *path);
// 	}
// }

tc_ResourceDep tico_plugin_resources_new_dep(const char *name, tc_Resource *res) {
	tc_ResourceDep dep = {0};
	strcpy(dep.name, name);
	dep.res = res;
	return dep;
}

TIC_API void tico_plugin_resources_push_dep(tc_Resource *res, const char *name, const char *resource_name) {

}

TIC_API void tico_plugin_resources_change_dep(tc_ResourceManager *manager, const char *name, const char *res_name);

static tc_Image* tico_lua_image_loader(lua_State *L, const char *path) {

}

#endif