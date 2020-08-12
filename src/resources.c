#include "tico.h"

#define TICO_PLUGIN_IMPLEMENTATION
#include "plugins/resource.h"

int tic_resources_init(tc_ResourceManager *manager) {
	// manager->resources = hashmap_create(MAX_RESOURCES);
	if (Core.lua.L) {
		lua_newtable(Core.lua.L);
		lua_setglobal(Core.lua.L, "__resources");
	}
	map_init(&manager->resources);
  map_init(&manager->resource_loaders);
  map_init(&manager->resource_stores);
  manager->path[0] = '\0';
// 	tic_resources_new_type(manager, "image");
// 	tic_resources_new_type(manager, "tileset");
// 	tic_resources_new_type(manager, "tilemap");
// 	tic_resources_new_type(manager, "font");
// 	tic_resources_new_type(manager, "sound");

	return 1;
}

int tic_resources_default_loaders(tc_ResourceManager *manager) {
  map_init(&manager->resource_loaders);
  map_init(&manager->resource_stores);
  map_init(&manager->plugins);
  map_set(&manager->plugins, "image", tic_plugin_resource_image());
  map_set(&manager->plugins, "tileset", tic_plugin_resource_tileset());
  map_set(&manager->plugins, "tilemap", tic_plugin_resource_tilemap());
  // map_set(&manager->resource_loaders, "image", tic_resources_image_loader);
  // map_set(&manager->resource_loaders, "tileset", tic_resources_tileset_loader);
  // map_set(&manager->resource_loaders, "tilemap", tic_resources_tilemap_loader);

  // map_set(&manager->resource_stores, "tileset", tic_resources_tileset_store);
  // map_set(&manager->resource_stores, "tilemap", tic_resources_tilemap_store);
  TRACELOG("default loaders");
}

enum {
	TIC_LOADER_IMAGE = 1,
	TIC_LOADER_TILESET,
	TIC_LOADER_TILEMAP
};

int tic_resources_load(tc_ResourceManager *manager, const char *path) {
	//if (!tic_resources_init(manager)) return 0;
  if (Core.lua.L) {
    lua_newtable(Core.lua.L);
    lua_setglobal(Core.lua.L, "__resources");
  }
  map_init(&manager->resources);
  // map_init(&manager->plugins);
  cJSON *json = tic_json_open(path);
	if (!json) return 0;

	strcpy(manager->path, path);

	cJSON *el = NULL;
	cJSON *resource = NULL;
	cJSON_ArrayForEach(el, json) {
		if (el->string) {
			tic_resources_new_type(manager, el->string);
			// int is_lua = tic_json_get_boolean(cJSON *const json, char *name)
			// TRACELOG("%s", el->string);
			int loader = 0;
			if (!strcmp(el->string, "image")) loader = TIC_LOADER_IMAGE;
			else if (!strcmp(el->string, "tileset")) loader = TIC_LOADER_TILESET;
			else if (!strcmp(el->string, "tilemap")) loader = TIC_LOADER_TILEMAP;
			if (loader == 0) continue;

      ResourceLoader *res_loader = (ResourceLoader*)map_get(&manager->resource_loaders, el->string);
      tc_ResourcePlugin *plugin = map_get(&manager->plugins, el->string);
      if (!plugin && !plugin->loader) continue;

			cJSON_ArrayForEach(resource, el) {
				const char *name = resource->string;
				// const char *path = tic_json_get_string(image, "path");
				int is_lua = tic_json_get_boolean(resource, "lua");
				tc_Resource res;
				list_init(&res.deps);
				strcpy(res.type, el->string);
				res.deps_count = 0;
				res.lua = 0;
				res.data = NULL;
				res.changed = tc_false;
				strcpy(res.name, name);
				res.path[0] = '\0';
				res.L = NULL;
				if (is_lua) {
					// res.type = TIC_RESOURCE_LUA;
					res.lua = 1;
					res.L = Core.lua.L;
					lua_settop(res.L, 0);
					lua_pushstring(res.L, el->string);
					lua_pushstring(res.L, name);
				}

				// TRACELOG("%s %s", el->string, name);
        tc_Resource reso = (plugin->loader)(manager, &res, resource);
				/*switch(loader) {
					case TIC_LOADER_IMAGE:
						tic_resources_image_loader(manager, &res, resource);
						break;
					case TIC_LOADER_TILESET:
						tic_resources_tileset_loader(manager, &res, resource);
						break;
					case TIC_LOADER_TILEMAP:
						tic_resources_tilemap_loader(manager, &res, resource);
						break;
					default:
						printf("No loader found for %s\n", name);
            }*/
				// if (loader == TIC_LOADER_IMAGE) tic_resources_image_loader(manager, &res, resource);
        // TRACELOG("%s", res.name);
				tic_resources_add(manager, el->string, name, &res);
			}
		}
	}
	// if (list) {
	// 	tic_resources_new_type(manager, "image");
	// 	cJSON *image = tic_json_get_object(list, "tileset-1");
	// 	int is_lua = tic_json_get_boolean(image, "lua");
	// 	// TRACELOG("%d", is_lua);
	// 	tc_Resource res;
	// 	res.type = is_lua ? TIC_RESOURCE_LUA : 0;
	// 	res.L = is_lua ? Core.lua.L : NULL;
	// 	if (is_lua) {
	// 		lua_pushstring(res.L, "image");
	// 		lua_pushstring(res.L, "tileset-1");
	// 	}
	// 	tic_resources_image_loader(manager, &res, image);
	// 	// const char *name = tic_json_get_string(image, "name");
	// 	tic_resources_add(manager, "image", "tileset-1", res);
	// 	// TRACELOG("%s", res.path);
	// }

	tic_json_delete(json);


	return 1;
}

void tic_resources_store(tc_ResourceManager *manager) {
	const char *key;
	map_iter_t iter = map_iter(&manager->resources);
	cJSON *res_json = tic_json_create();
	while((key = map_next(&manager->resources, &iter))) {
		tc_Resource *res = map_get(&manager->resources, key);
		// TRACELOG("%d %s", res->type & TIC_RESOURCE_IMAGE, key);
		cJSON *category = tic_json_get_object(res_json, res->type);
		if (!category) category = tic_json_set_object(res_json, res->type, NULL);

		cJSON *jres = tic_json_create();
		tic_json_set_string(jres, "path", res->path);
		tic_json_set_boolean(jres, "lua", res->lua);
		tic_json_set_object(category, res->name, jres);

		// if (!strcmp(res->type, "image")) {
		// 	cJSON *image = tic_json_get_object(res_json, "image");
		// 	if (!image) image = tic_json_set_object(res_json, "image", NULL);

		// 	cJSON *jres = tic_json_create();
		// 	tic_json_set_string(jres, "path", res->path);
		// 	tic_json_set_boolean(jres, "lua", res->type & TIC_RESOURCE_LUA);
		// 	tic_json_set_object(image, res->name, jres);
		// 	// if (tic_json_is_object(json, char *name))
		// } else if (res->type & TIC_RESOURCE_TILESET) {
		// 	cJSON *tileset = tic_json_get_object(res_json, "tileset");
		// 	if (!tileset) tileset = tic_json_set_object(res_json, "tileset", NULL);

		// 	cJSON *jres = tic_json_create();
		// 	tic_json_set_string(jres, "path", res->path);
		// 	tic_json_set_boolean(jres, "lua", res->type & TIC_RESOURCE_LUA);
		// 	tic_json_set_object(tileset, res->name, jres);
		// } else if (res->type & TIC_RESOURCE_TILEMAP) {
		// 	cJSON *tilemap = tic_json_get_object(res_json, "tilemap");
		// 	if (!tilemap) tilemap = tic_json_set_object(res_json, "tilemap", NULL);

		// 	cJSON *jres = tic_json_create();
		// 	tic_json_set_string(jres, "path", res->path);
		// 	tic_json_set_boolean(jres, "lua", res->type & TIC_RESOURCE_LUA);
		// 	tic_json_set_object(tilemap, res->name, jres);
		// }
	}

	tic_json_save(manager->path, res_json);
	tic_json_delete(res_json);
}

// Push a loader
void tic_resources_push_loader(tc_ResourceManager *manager, const char *type, ResourceLoader loader) {
  map_set(&manager->resource_loaders, type, loader);
}

void tic_resources_push_storer(tc_ResourceManager *manager, const char *type) {
}

void tic_resources_new_type(tc_ResourceManager *manager, const char *type) {
// 	map_resource_t res;
// 	map_init(&res);
// 	map_set(&manager->resources, type, res);
	if (Core.lua.L) {
		// TRACELOG("%s", type);
		lua_getglobal(Core.lua.L, "__resources");
		lua_getfield(Core.lua.L, -1, type);
		if (!lua_isnil(Core.lua.L, -1)) return;
		lua_getglobal(Core.lua.L, "__resources");
		lua_newtable(Core.lua.L);
		lua_setfield(Core.lua.L, -2, type);
	}
}

tc_bool tic_resources_add(tc_ResourceManager *manager, const char *type, const char *name, tc_Resource *resource) {
	// hashmap_item *item = hashmap_get(manager->resources, type);
	// if (item && item->data) {
	// 	hashmap *map = (hashmap*)item->data;
	// 	hashmap_item *resource = hashmap_get(*map, name);
	// 	if (resource) return;
	// 	// else hashmap_set()
	// }
	// tc_Resource *resource = map_get()
	// TRACELOG("%s %s", type, name);
// 	map_resource_t *res = map_get(&manager->resources, type);
  int size = strlen(type) + strlen(name) + 2;
  char nm[size];
  sprintf(nm, "%s##%s", type, name);
  tc_Resource *res = map_get(&manager->resources, type);
	if (res) return 0;

	// TRACELOG("%s %s", resource->name, name);
	if (!resource->name[0]) {
		// resource->name = calloc(strlen(name), 1);
		strcpy(resource->name, name);
	}
// 	map_set(res, name, *resource);
//   int size = strlen(type) + strlen(name) + 2;
//   char nm[size];
//   sprintf(nm, "%s##%s", type, name);
//   TRACELOG("%s", nm);
  map_set(&manager->resources, nm, *resource);
	if (resource->lua) {
		int top = lua_gettop(resource->L);
		// tc_Image *image = lua_touserdata(resource.L, 3);
		// if (image) TRACELOG("userdata %d", top);
		// TRACELOG("%s %s %d", type, name, top);
		lua_getglobal(resource->L, "__resources");
		lua_getfield(resource->L, -1, type);
		// top = lua_gettop(resource.L);
		// TRACELOG("%d", top);
		if (!lua_isnil(resource->L, -1)) {
			// TRACELOG("%s %s %d", type, name, top);
			lua_pushvalue(resource->L, top);
			lua_setfield(resource->L, -2, name);
		}
	}

	return 1;
}

tc_Resource* tic_resources_get(tc_ResourceManager *manager, const char *type, const char *name) {
	tc_Resource *res = NULL;
// 	map_resource_t *res_type = map_get(&manager->resources, type);
// 	if (res_type) {
// 		res = map_get(res_type, name);
// 		if (res->type & TIC_RESOURCE_LUA) {
// 			lua_getglobal(res->L, "__resources");
// 			lua_getfield(res->L, -1, type);
// 			if (!lua_isnil(res->L, -1)) {
// 				lua_getfield(res->L, -1, name);
// 				lua_pushvalue(res->L, -1);
// 			} else lua_pushnil(res->L);
// 		}
// 	}
  int size = strlen(type) + strlen(name) + 2;
  char nm[size];
  sprintf(nm, "%s##%s", type, name);
  res = map_get(&manager->resources, nm);
  // TRACELOG("%s %p", nm, res);
  if (!res) return NULL;

  if (res->lua) {
    lua_getglobal(res->L, "__resources");
    lua_getfield(res->L, -1, type);
    if (!lua_isnil(res->L, -1)) {
      lua_getfield(res->L, -1, name);
      lua_pushvalue(res->L, -1);
    } else lua_pushnil(res->L);
  }

	return res;
}

tc_ResourceDep tic_resources_new_dep(const char *name, tc_Resource *res) {
	tc_ResourceDep dep = {0};
	// dep.name = malloc(strlen(name));
	strcpy(dep.name, name);
	dep.res = res;
	return dep;
}

TIC_API void tic_resources_push_dep(tc_Resource *res, const char *name, const char *resource_name) {

}

TIC_API void tic_resources_change_dep(tc_ResourceManager *manager, const char *name, const char *res_name);

static tc_Image* tic_lua_image_loader(lua_State *L, const char *path) {

}

tc_Resource tic_resources_image_loader(tc_ResourceManager *manager, tc_Resource *res, cJSON *json) {
	tc_Image *image = NULL;
	const char *path = tic_json_get_string(json, "path");
	if (!strcmp(res->type, "image"))	{
		image = lua_newuserdata(res->L, sizeof(*image));
		luaL_setmetatable(res->L, LUA_CLASS(tc_Image));
	}
	else image = malloc(sizeof(*image));

	if (image) {
		// res->path = malloc(strlen(path));
		strcpy(res->path, path);
		*image = tic_image_load(path);
		res->data = image;
		// res->type |= TIC_RESOURCE_IMAGE;
		strcpy(res->type, "image");
	}

	return *res;
}

tc_Resource tic_resources_tileset_loader(tc_ResourceManager *manager, tc_Resource *res, cJSON *json) {
	tc_Tileset *tileset = NULL;
	const char *path = tic_json_get_string(json, "path");
	// const char *image_name = tic_json_get_string(json, "image");
	// tc_Resource *image_res = NULL;
	// TRACELOG("%s %s", path, image_name);
	// if (image_name) image_res = tic_resources_get(manager, "image", image_name);
	// if (!image_res) {
	// 	ERRLOG("Failed to load tileset %s, image %s not found", path, image_name);
	// 	return NULL;
	// }
	// TRACELOG("%s", path);
	// TRACELOG("%s", path);

	cJSON *root = tic_json_open(path);
	int tilew = tic_json_get_number(root, "tilewidth");
	int tileh = tic_json_get_number(root, "tileheight");
	const char *image_name = tic_json_get_string(root, "image");
	tc_Resource *resource = NULL;
	if (image_name) resource = tic_resources_get(manager, "image", image_name);
	// TRACELOG("%s", image_name);
	// TRACELOG("%p", resource);
	if (!resource) {
		ERRLOG("Failed to load %s, image %s not found", path, image_name);
		tic_json_delete(root);
		return *res;
	}

	if (res->lua) {
		tileset = lua_newuserdata(res->L, sizeof(*tileset));
		luaL_setmetatable(res->L, LUA_CLASS(tc_Tileset));
	} else tileset = malloc(sizeof(*tileset));

	if (tileset) {
		// TRACELOG("teste");
		// res->path = malloc(strlen(path));
		strcpy(res->path, path);

		// res->deps[res->deps_count++] = resource;
		tc_ResourceDep dep = tic_resources_new_dep("image", resource);
		// dep.next = NULL;
		list_push(&res->deps, dep);
		// TRACELOG("%s %s", dep.name, dep.res->name);
		tc_Image *image = resource->data;
		*tileset = tic_tileset_create(image, tilew, tileh);
		cJSON *bitmask = tic_json_get_array(root, "bitmasks");
		cJSON *el;
		int i = 0;
		if (bitmask) {
			cJSON_ArrayForEach(el, bitmask) {
				if (i >= tileset->tilecount) break;
				tileset->bitmasks[i] = el->valueint;
				// TRACELOG("%d %d", i, el->valueint);
				i++;
			}
		}

		tic_json_delete(root);
		res->data = tileset;
		// res->type |= TIC_RESOURCE_TILESET;
		strcpy(res->type, "tileset");
	}

	return *res;
}

tc_Resource tic_resources_tilemap_loader(tc_ResourceManager *manager, tc_Resource *res, cJSON *json) {
	tc_Tilemap *tilemap = NULL;
	const char *path = tic_json_get_string(json, "path");

	cJSON *root = tic_json_open(path);
	int width = tic_json_get_number(root, "width");
	int height = tic_json_get_number(root, "height");
	const char *tileset_name = tic_json_get_string(root, "tileset");
	tc_Resource *resource = NULL;
	if (tileset_name) resource = tic_resources_get(manager, "tileset", tileset_name);
	// TRACELOG("%s", image_name);
	// TRACELOG("%p", resource);
	if (!resource) {
		ERRLOG("Failed to load %s, tileset %s not found", path, tileset_name);
		tic_json_delete(root);
		return (tc_Resource){0};
	}

	if (res->lua) {
		tilemap = lua_newuserdata(res->L, sizeof(*tilemap));
		luaL_setmetatable(res->L, LUA_CLASS(tc_Tilemap));
	} else tilemap = malloc(sizeof(*tilemap));

	if (tilemap) {
		// TRACELOG("teste");
		// res->path = malloc(strlen(path));
		// strcpy(res->name, root->string);
		strcpy(res->path, path);
		tc_ResourceDep dep = tic_resources_new_dep("tileset", resource);
		list_push(&res->deps, dep);
		// res->deps[res->deps_count++] = resource;
		tc_Tileset *tileset = resource->data;
		*tilemap = tic_tilemap_create(tileset, width, height);
		cJSON *data = tic_json_get_array(root, "data");
		cJSON *el;
		int i = 0;
		cJSON_ArrayForEach(el, data) {
			if (i > tilemap->count) break;
			tilemap->data[i] = el->valueint;
			// TRACELOG("%d %d", i, el->valueint);
			i++;
		}

		tic_json_delete(root);
		res->data = tilemap;
		strcpy(res->type, "tilemap");
	}

	return *res;
}

cJSON* tic_resources_tileset_store(tc_ResourceManager *manager, tc_Resource *res) {
	// TRACELOG("%s", name);
// 	map_resource_t *res = map_get(&manager->resources, "tileset");
// 	if (!res) return;

  int size = strlen("tileset##") + strlen(res->name);
  char nm[size];
  sprintf(nm, "tileset##%s", res->name);

	// tc_Resource *resource = map_get(&manager->resources, nm);
	if (!res) return NULL;

	cJSON *root = NULL;
	if (res->path[0]) root = tic_json_open(res->path);
	else root = tic_json_create();

	tc_Tileset *tileset = res->data;
	int deps_count = res->deps_count;
	tic_json_set_number(root, "tilewidth", tileset->tilesize.x);
	tic_json_set_number(root, "tileheight", tileset->tilesize.y);
	tic_json_set_number(root, "imagewidth", tileset->image->width);
	tic_json_set_number(root, "imageheight", tileset->image->height);
	// if (editor->image_name) tic_json_set_string(root, "image", editor->image_name);
	tic_json_set_number(root, "columns", tileset->columns);
	tic_json_set_number(root, "tilecount", tileset->tilecount);

	// if (deps_count > 0) {
	// 	tic_json_set_string(root, "image", resource->deps[0]->name);
	// }

	list_iter_t iter = list_iter(&res->deps);
	tc_ResourceDep *dep;
	while((dep = list_next(&iter))) {
		tic_json_set_string(root, dep->name, dep->res->name);
	}

	// cJSON *bitmask = tic_json_create_array();
	// tic_json_set_object(root, "bitmasks", bitmask);
	cJSON *bitmask = tic_json_get_array(root, "bitmasks");
	if (bitmask) {
		for (int i = 0; i < tileset->tilecount; i++) {
			cJSON_DeleteItemFromArray(bitmask, 0);
		}
	} else {
		bitmask = tic_json_create_array();
		tic_json_set_array(root, "bitmasks", bitmask);
	}

	for (int i = 0; i < tileset->tilecount; i++) {
		cJSON *n = cJSON_CreateNumber(tileset->bitmasks[i]);
		cJSON_InsertItemInArray(bitmask, i, n);
	}

	// char val[25];
	// memset(val, 0, 25);
	// TRACELOG("%d", tileset->tilecount);
	if (!res->path[0]) {
		// resource->path = malloc(strlen("tileset.json"));
		strcpy(res->path, "tileset_");
		strcat(res->path, res->name);
		strcat(res->path, ".json");
	}

	tic_json_save(res->path, root);
	// tic_json_delete(root);
	return root;
}

cJSON* tic_resources_tilemap_store(tc_ResourceManager *manager, tc_Resource *resource) {
// 	map_resource_t *res = map_get(&manager->resources, "tilemap");
// 	if (!res) return;

	int size = strlen("tilemap##") + strlen(resource->name);
  char nm[size];
  sprintf(nm, "tilemap##%s", resource->name);
	// TRACELOG("%s", nm);

	// tc_Resource *resource = map_get(&manager->resources, nm);
	if (!resource) return NULL;


	cJSON *root = tic_json_create();
	// if (resource->path[0]) root = tic_json_open(resource->path);
	// else root = tic_json_create();

	tc_Tilemap *tilemap = resource->data;
	int deps_count = resource->deps_count;
	tic_json_set_number(root, "width", tilemap->width);
	tic_json_set_number(root, "height", tilemap->height);
	// if (editor->image_name) tic_json_set_string(root, "image", editor->image_name);
	tic_json_set_number(root, "count", tilemap->count);
	tic_json_set_number(root, "tilewidth", tilemap->grid.x);
	tic_json_set_number(root, "tileheight", tilemap->grid.y);
	// tic_json_set_number(root, "tilecount", tileset->tilecount);

	// if (deps_count > 0) {
	// 	tic_json_set_string(root, "tileset", resource->deps[0]->name);
	// }

	list_iter_t iter = list_iter(&resource->deps);
	tc_ResourceDep *dep;
	while((dep = list_next(&iter))) {
		tic_json_set_string(root, dep->name, dep->res->name);
	}

	// cJSON *bitmask = tic_json_create_array();
	// tic_json_set_object(root, "bitmasks", bitmask);
	cJSON *data = tic_json_get_array(root, "data");
	if (data) {
		for (int i = 0; i < tilemap->count; i++) {
			cJSON_DeleteItemFromArray(data, 0);
		}
	} else {
		data = tic_json_create_array();
		tic_json_set_array(root, "data", data);
	}

	for (int i = 0; i < tilemap->count; i++) {
		cJSON *n = cJSON_CreateNumber(tilemap->data[i]);
		cJSON_InsertItemInArray(data, i, n);
	}

	// char val[25];
	// memset(val, 0, 25);
	// TRACELOG("%d", tileset->tilecount);
	if (!resource->path[0]) {
		// resource->path = malloc(strlen("tilemap.json"));
		strcpy(resource->path, "tilemap.json");
	}

	tic_json_save(resource->path, root);
	
	return root;
}

void tic_resources_save(tc_ResourceManager *manager) {
  
}

tc_bool tic_resources_add_image(tc_ResourceManager *manager, const char *name, tc_Image *image) {
	tc_Resource res = {0};
	res.data = image;
	// res.type |= TIC_RESOURCE_IMAGE;
	strcpy(res.type, "image");

	return tic_resources_add(manager, "image", name, &res);
}

tc_Image* tic_resources_get_image(tc_ResourceManager *manager, const char *name) {
	tc_Resource *res = tic_resources_get(manager, "image", name);
	if (res) return res->data;

	return NULL;
}

void tic_resource_remove_image(tc_ResourceManager *manager, const char * name) {
	tc_Resource *res = tic_resources_get(manager, "image", name);
// 	map_resource_t *type = map_get(&manager->resources, "image");
	if (!res) return;
// 	map_remove(type, name);
	tc_Image *image = res->data;
	tic_image_destroy(image);
}
