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
  int deps_count;
  int id;
  int changed;
  int lua;
  char type[64];
  char uuid[RES_UUID_SIZE+1];
  char name[256];
  char path[256];
  void *data;
  lua_State *L;
#ifdef WREN_LANG
  WrenVM *vm;
#endif
  list_resdep_t deps;
  map_deps_t dep;
};

typedef struct tc_ResourceManager tc_ResourceManager;

typedef tc_Resource(*PluginResourceLoader)(tc_ResourceManager*, tc_Resource*, cJSON*);
typedef cJSON*(*PluginResourceStore)(tc_ResourceManager*, tc_Resource*);

struct tc_ResourcePlugin {
	PluginResourceLoader loader;
	PluginResourceStore store;
};

TIC_API tc_ResourcePlugin tico_plugin_resource_create(PluginResourceLoader loader, PluginResourceStore store);

/*****************
 * Image
 *****************/

TIC_API tc_ResourcePlugin tico_plugin_resource_image();
TIC_API tc_Resource tico_plugin_resource_image_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json);
TIC_API cJSON* tico_plugin_resource_image_store(tc_ResourceManager *manager, tc_Resource *res);

/****************
 * Tileset
 ****************/

TIC_API tc_ResourcePlugin tico_plugin_resource_tileset();
TIC_API tc_Resource tico_plugin_resource_tileset_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json);
TIC_API cJSON* tico_plugin_resource_tileset_store(tc_ResourceManager *manager, tc_Resource *res);

/****************
 * Tilemap
 ****************/

TIC_API tc_ResourcePlugin tico_plugin_resource_tilemap();
TIC_API tc_Resource tico_plugin_resource_tilemap_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json);
TIC_API cJSON* tico_plugin_resource_tilemap_store(tc_ResourceManager *manager, tc_Resource *res);

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
};

TIC_API tc_bool tico_plugin_resources_init(tc_ResourceManager *manager);
TIC_API int tico_plugin_resources_default_loaders(tc_ResourceManager *manager);
TIC_API tc_bool tico_plugin_resources_load(tc_ResourceManager *manager, const char *type, const char *name, const char *path);
TIC_API void tico_plugin_resources_store(tc_ResourceManager *manager);
TIC_API tc_bool tico_plugin_resources_destroy(tc_ResourceManager *manager);

TIC_API int tico_plugin_resources_check_json_deps(tc_ResourceManager *manager, cJSON *json);
TIC_API void tico_plugin_resources_update_missing(tc_ResourceManager *manager);
TIC_API tc_ResourceDep tico_plugin_resources_new_dep(const char *name, tc_Resource *res);
TIC_API void tico_plugin_resources_push_dep(tc_Resource *res, const char *name, const char *resource_name);
TIC_API void tico_plugin_resources_change_dep(tc_ResourceManager *manager, const char *name, const char *res_name);
TIC_API void tico_plugin_resources_new_type(tc_ResourceManager *manager, const char *type);

TIC_API void tico_plugin_resources_update_resource(tc_Resource *resource);

TIC_API tc_bool tico_plugin_resources_add(tc_ResourceManager *manager, const char *type, const char *name, tc_Resource *resource);

TIC_API tc_Resource* PUBLIC_API(tico_plugin_resources_get_by_key, const char *key);
TIC_API tc_Resource* PUBLIC_API(tico_plugin_resources_get_by_uuid, const char *uuid);
TIC_API tc_Resource* PUBLIC_API(tico_plugin_resources_get, const char *type, const char *name);
TIC_API tc_Resource* INTERNAL_API(tico_plugin_resources_get_by_key, tc_ResourceManager *manager, const char *key);
TIC_API tc_Resource* INTERNAL_API(tico_plugin_resources_get_by_uuid, tc_ResourceManager *manager, const char *uuid);
TIC_API tc_Resource* INTERNAL_API(tico_plugin_resources_get, tc_ResourceManager *manager, const char *type, const char *name);
#endif // TICO_RESOURCES_H

#if defined(TICO_PLUGIN_IMPLEMENTATION)


#include "lua/resources.h"
// #define RES_KEY(type, name) #type"//"#name

tc_Resource tico_plugin_resources_create_resource(const char *type, const char *name) {
	tc_Resource res = {0};
	memset(&res, 0, sizeof(res));
	// tico_utils_generate_uuid(res.uuid, 8);
	strcpy(res.type, type);
	strcpy(res.name, name);
	map_init(&res.dep);

	return res;
}

int tico_plugin_resources_init(tc_ResourceManager *manager) {
	manager->lua = NULL;

	tico_plugin_resources_default_loaders(manager);

	tc_Plugin *plugin = tico_plugin_get("lua");
	if (plugin) {
		tc_Lua *lua = plugin->data;
		lua_newtable(lua->L);
		lua_setglobal(lua->L, "__resources");
		manager->lua = lua;
		tico_plugin_lua_add_lib("resources", luaopen_resources);
	}
  manager->path[0] = '\0';
  map_init(&manager->keys);
  map_init(&manager->resources);
  map_init(&manager->missing);
	// strcpy(manager->path, path ? path : "resources.json");
	strcpy(manager->path, PROJECT_FILE);

	cJSON *json = tico_json_open(manager->path);
	if (!json) return 0;

	cJSON *resources = tico_json_get_object(json, "resources");
	if (!resources) return 0;


	cJSON *el = NULL;
	cJSON *resource = NULL;
	cJSON_ArrayForEach(el, resources) {
		if (el->string) {
      tc_ResourcePlugin *plugin = map_get(&manager->plugins, el->string);
      if (!plugin && !plugin->loader) continue;

			cJSON_ArrayForEach(resource, el) {
				const char *name = resource->string;
				// TRACELOG("%s %s", el->string, name);
				tico_plugin_resources_load(manager, el->string, name, tico_json_get_string(resource, "path"));
			}
		}
	}

	tico_json_delete(json);

	return 1;
}

int tico_plugin_resources_default_loaders(tc_ResourceManager *manager) {
  map_init(&manager->plugins);
  map_set(&manager->plugins, "image", tico_plugin_resource_image());
  map_set(&manager->plugins, "tileset", tico_plugin_resource_tileset());
  map_set(&manager->plugins, "tilemap", tico_plugin_resource_tilemap());
  // map_set(&manager->plugins, "sprite", tico_plugin_resource_sprite());
  TRACELOG("default loaders");
}

enum {
	TIC_LOADER_IMAGE = 1,
	TIC_LOADER_TILESET,
	TIC_LOADER_TILEMAP
};

int tico_plugin_resources_load(tc_ResourceManager *manager, const char *type, const char *name, const char *path) {
	if (!tico_filesystem_file_exists((const unsigned char*)path)) return 0;

	cJSON *json = NULL;
	if (tico_json_is_valid(path)) json = tico_json_open(path);

	tc_ResourcePlugin *plugin = map_get(&manager->plugins, type);

	if (!plugin) {
		if (json) tico_json_delete(json);
		return 0;
	}
	// TRACELOG("%s", type);

	tc_Resource res = tico_plugin_resources_create_resource(type, name);
	if (manager->lua) {
		res.lua = 1;
		res.L = manager->lua->L;
	}
	lua_settop(res.L, 0);
	strcpy(res.path, path);

	if (json) {
		int sz = strlen(type) + strlen(name) + 2;
		char nm[sz];
		sprintf(nm, "%s//%s", type, name);

		if (!tico_plugin_resources_check_json_deps(manager, json)) {
			map_set(&manager->missing, nm, (char*)path);
			tico_json_delete(json);
			return 0;
		}
		cJSON *deps = tico_json_get_object(json, "dependencies");
		cJSON *dep = NULL;
		cJSON_ArrayForEach(dep, deps) {
			// tc_Resource *resdep = map_get(&manager->resources, dep->valuestring);
			tc_Resource *resdep = tico_plugin_resources_get_by_key_internal(manager, dep->valuestring);
			// if (!resdep) res.status |= TIC_RESOURCE_MISSING_DEP;
			tc_ResourceDep rdep = tico_plugin_resources_new_dep(dep->string, resdep);
			strcpy(rdep.res_key, dep->valuestring);
			map_set(&res.dep, dep->string, rdep);
			// TRACELOG("%s", dep->string);
		}

		// if (res.status & TIC_RESOURCE_MISSING_DEP) {
		// 	map_set(&manager->resources, nm, res);
		// 	map_set(&manager->missing, nm, path);
		// 	tico_json_delete(json);
		// 	return 0;
		// }
		char **dp = map_get(&manager->missing, nm);
		if (dp) map_remove(&manager->missing, nm); 

		// exit(1);
	}

	plugin->loader(manager, &res, json);
	if (res.status & TIC_RESOURCE_LOADED) tico_plugin_resources_add(manager, type, name, &res);

	tico_plugin_resources_update_missing(manager);

	return 1;
}

void tico_plugin_resources_store(tc_ResourceManager *manager) {
	const char *key;
	map_iter_t iter = map_iter(&manager->resources);
	cJSON *res_json = tico_json_create();
	while((key = map_next(&manager->resources, &iter))) {
		tc_Resource *res = map_get(&manager->resources, key);
		tc_ResourcePlugin *plugin = map_get(&manager->plugins, res->type);
		cJSON *jdata = NULL;
		if (plugin->store) jdata = plugin->store(manager, res);
		if (jdata) {
			cJSON *deps = tico_json_get_object(jdata, "dependencies");
			if (deps) tico_json_delete(deps);
			deps = tico_json_set_object(jdata, "dependencies", NULL);
			const char *dep_name;
			map_iter_t diter = map_iter(&res->dep);
			while ((dep_name = map_next(&res->dep, &diter))) {
				tc_ResourceDep *rdp = map_get(&res->dep, dep_name);
				tico_json_set_string(deps, dep_name, rdp->res_key);
			}
			tico_json_save(res->path, jdata);
			tico_json_delete(jdata);
		}
		// TRACELOG("%d %s", res->type & TIC_RESOURCE_IMAGE, key);
		cJSON *category = tico_json_get_object(res_json, res->type);
		if (!category) category = tico_json_set_object(res_json, res->type, NULL);

		cJSON *jres = tico_json_create();
		tico_json_set_string(jres, "path", res->path);
		tico_json_set_boolean(jres, "lua", res->lua);
		tico_json_set_object(category, res->name, jres);
	}

	tico_json_save(manager->path, res_json);
	tico_json_delete(res_json);
}

tc_bool tico_plugin_resources_add(tc_ResourceManager *manager, const char *type, const char *name, tc_Resource *resource) {
	if (~resource->status & TIC_RESOURCE_LOADED) return 0;
  int size = strlen(type) + strlen(name) + 2;
  char key[size];
  sprintf(key, "%s//%s", type, name);
  char **uuid = map_get(&manager->keys, key);

	if (uuid) return 0;


	strcpy(resource->type, type);
	strcpy(resource->name, name);
	// char uuid[8];
	tico_utils_generate_uuid(resource->uuid, 8);
		// TRACELOG("%s %s", key, resource->uuid);
	while(map_get(&manager->resources, resource->uuid)) {
		tico_utils_generate_uuid(resource->uuid, 8);
	}
	// map_set(&manager->keys, key, resource->uuid);
	map_set(&manager->resources, resource->uuid, *resource);
	if (resource->lua) {
		int top = lua_gettop(resource->L);
		lua_getglobal(resource->L, "__resources");
		lua_pushvalue(resource->L, top);
		lua_setfield(resource->L, -2, resource->uuid);
	}

	tc_Resource* res = map_get(&manager->resources, resource->uuid);
	map_set(&manager->keys, key, res->uuid);


	return 1;
}

tc_Resource* PUBLIC_API(tico_plugin_resources_get, const char *type, const char *name) {
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
	tc_Resource* res = map_get(&manager->resources, uuid);
	if (!res) return NULL;

	if (res->lua) {
		lua_getglobal(res->L, "__resources");
		lua_getfield(res->L, -1, uuid);
		if (!lua_isnil(res->L, -1)) lua_pushvalue(res->L, -1);
		else lua_pushnil(res->L);
	}

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
	cJSON *deps = tico_json_get_object(json, "dependencies");
	if (!deps) return 0;

	cJSON *dep = NULL;
	cJSON_ArrayForEach(dep, deps) {
		tc_Resource *res = tico_plugin_resources_get_by_key_internal(manager, dep->valuestring);
		if (!res) return 0; 
	}

	return 1;
}

void tico_plugin_resources_update_missing(tc_ResourceManager *manager) {
	map_iter_t iter = map_iter(&manager->missing);
	const char *key;
	while((key = map_next(&manager->missing, &iter))) {
		char **path = map_get(&manager->missing, key);
		if (!path) continue;
		char *name = strstr(key, "//");
		int type_sz = name - key;
		char type[type_sz];
		memcpy(type, key, type_sz);
		name += 2;
		// TRACELOG("%s %s %s", type, name, *path);
		tico_plugin_resources_load(manager, type, name, *path);
	}
}

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

/*********************
 * Resource Plugins
 *********************/

tc_ResourcePlugin tico_plugin_resource_init(PluginResourceLoader loader, PluginResourceStore store) {
	tc_ResourcePlugin plugin = {0};
	plugin.loader = loader;
	plugin.store = store;

	return plugin;
}

/*****************
 * Image
 *****************/

tc_ResourcePlugin tico_plugin_resource_image() {
	return tico_plugin_resource_init(tico_plugin_resource_image_loader, tico_plugin_resource_image_store);
}

tc_Resource tico_plugin_resource_image_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json) {
	tc_Image *image = NULL;
	// const char *path = tico_json_get_string(json, "path");
	// TRACELOG("%s", path);
	const char *path = res->path;

	// int lua = tico_json_get_boolean(json, "lua");
	if (res->lua)	{
		image = lua_newuserdata(res->L, sizeof(*image));
		luaL_setmetatable(res->L, LUA_CLASS(tc_Image));
	}
	else image = malloc(sizeof(*image));



	if (image) {
		// res->path = malloc(strlen(path));
		*image = tico_image_load(path);
		res->data = image;
		if (!image->texture.id) res->status = TIC_RESOURCE_ERROR;
		// res->type |= TIC_RESOURCE_IMAGE;
	} else {
		res->status = TIC_RESOURCE_ERROR;
	}
	if (!res->status) res->status |= TIC_RESOURCE_LOADED;

	return *res;
}

cJSON* tico_plugin_resource_image_store(tc_ResourceManager *manager, tc_Resource *res) {
	return NULL;
}

/****************
 * Tileset
 ****************/

tc_ResourcePlugin tico_plugin_resource_tileset() {
	return tico_plugin_resource_init(tico_plugin_resource_tileset_loader, tico_plugin_resource_tileset_store);
}

tc_Resource tico_plugin_resource_tileset_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json) {
	tc_Tileset *tileset = NULL;
	const char *path = res->path;
	cJSON *root = json;
	int tilew = tico_json_get_number(root, "tilewidth");
	int tileh = tico_json_get_number(root, "tileheight");;

	if (res->lua) {
		tileset = lua_newuserdata(res->L, sizeof(*tileset));
		luaL_setmetatable(res->L, LUA_CLASS(tc_Tileset));
	} else tileset = malloc(sizeof(*tileset));

	if (tileset) {
		tc_ResourceDep *dep = map_get(&res->dep, "image");
		tc_Image *image = dep->res->data;
		*tileset = tico_tileset_create(image, tilew, tileh);
		cJSON *bitmask = tico_json_get_array(root, "bitmasks");
		cJSON *el;
		int i = 0;
		if (bitmask) {
			cJSON_ArrayForEach(el, bitmask) {
				if (i >= tileset->tilecount) break;
				tileset->bitmasks[i] = el->valueint;
				i++;
			}
		}
		res->data = tileset;
		res->status = TIC_RESOURCE_LOADED;
	}

	return *res;
}

cJSON* tico_plugin_resource_tileset_store(tc_ResourceManager *manager, tc_Resource *res) {
	int size = strlen("tileset##") + strlen(res->name);
  char nm[size];
  sprintf(nm, "tileset##%s", res->name);

	if (!res) return NULL;

	cJSON *root = tico_json_create();

	tc_Tileset *tileset = res->data;
	int deps_count = res->deps_count;
	tico_json_set_number(root, "tilewidth", tileset->tilesize.x);
	tico_json_set_number(root, "tileheight", tileset->tilesize.y);
	tico_json_set_number(root, "imagewidth", tileset->image->width);
	tico_json_set_number(root, "imageheight", tileset->image->height);
	tico_json_set_number(root, "columns", tileset->columns);
	tico_json_set_number(root, "tilecount", tileset->tilecount);

	cJSON *bitmask = tico_json_get_array(root, "bitmasks");
	if (bitmask) {
		for (int i = 0; i < tileset->tilecount; i++) {
			cJSON_DeleteItemFromArray(bitmask, 0);
		}
	} else {
		bitmask = tico_json_create_array();
		tico_json_set_array(root, "bitmasks", bitmask);
	}

	for (int i = 0; i < tileset->tilecount; i++) {
		cJSON *n = cJSON_CreateNumber(tileset->bitmasks[i]);
		cJSON_InsertItemInArray(bitmask, i, n);
	}

	if (!res->path[0]) {
		strcpy(res->path, "tileset_");
		strcat(res->path, res->name);
		strcat(res->path, ".json");
	}

	// tico_json_save(res->path, root);
	return root;
}

/****************
 * Tilemap
 ****************/

tc_ResourcePlugin tico_plugin_resource_tilemap() {
	return tico_plugin_resource_init(tico_plugin_resource_tilemap_loader, tico_plugin_resource_tilemap_store);
}

tc_Resource tico_plugin_resource_tilemap_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json) {
	tc_Tilemap *tilemap = NULL;
	// const char *path = tico_json_get_string(json, "path");
	const char *path = res->path;

	// TRACELOG("%s", path);

	// cJSON *root = tico_json_open(path);
	cJSON *root = json;
	int width = tico_json_get_number(root, "width");
	int height = tico_json_get_number(root, "height");
	const char *tileset_name = tico_json_get_string(root, "tileset");
	if (res->lua) {
		tilemap = lua_newuserdata(res->L, sizeof(*tilemap));
		luaL_setmetatable(res->L, LUA_CLASS(tc_Tilemap));
	} else tilemap = malloc(sizeof(*tilemap));

	if (tilemap) {
		tc_ResourceDep *dep = map_get(&res->dep, "tileset");
		tc_Tileset *tileset = dep->res->data;
		*tilemap = tico_tilemap_create(tileset, width, height);
		cJSON *data = tico_json_get_array(root, "data");
		cJSON *el;
		int i = 0;
		cJSON_ArrayForEach(el, data) {
			if (i > tilemap->count) break;
			tilemap->data[i] = el->valueint;
			i++;
		}

		// tico_json_delete(root);
		res->data = tilemap;
		res->status = TIC_RESOURCE_LOADED;
		// strcpy(res->type, "tilemap");
	}

	return *res;
}

cJSON* tico_plugin_resource_tilemap_store(tc_ResourceManager *manager, tc_Resource *resource) {
	// int size = strlen("tilemap//") + strlen(resource->name);
 //  char nm[size];
 //  sprintf(nm, "tilemap//%s", resource->name);
	// // TRACELOG("%s", nm);

	if (!resource) return NULL;


	cJSON *root = tico_json_create();

	tc_Tilemap *tilemap = resource->data;
	int deps_count = resource->deps_count;
	tico_json_set_number(root, "width", tilemap->width);
	tico_json_set_number(root, "height", tilemap->height);
	tico_json_set_number(root, "count", tilemap->count);
	tico_json_set_number(root, "tilewidth", tilemap->grid.x);
	tico_json_set_number(root, "tileheight", tilemap->grid.y);


	// list_iter_t iter = list_iter(&resource->deps);
	// tc_ResourceDep *dep;
	// while((dep = list_next(&iter))) {
	// 	tico_json_set_string(root, dep->name, dep->res->name);
	// }

	cJSON *data = tico_json_get_array(root, "data");
	if (data) {
		for (int i = 0; i < tilemap->count; i++) {
			cJSON_DeleteItemFromArray(data, 0);
		}
	} else {
		data = tico_json_create_array();
		tico_json_set_array(root, "data", data);
	}

	for (int i = 0; i < tilemap->count; i++) {
		cJSON *n = cJSON_CreateNumber(tilemap->data[i]);
		cJSON_InsertItemInArray(data, i, n);
	}

	if (!resource->path[0]) {
		strcpy(resource->path, "tilemap.json");
	}

	// tico_json_save(resource->path, root);
	
	return root;
}

#endif