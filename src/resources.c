#include "tico.h"

int tic_resources_init(tc_ResourceManager *manager) {
	// manager->resources = hashmap_create(MAX_RESOURCES);
	if (Core.lua.L) {
		lua_newtable(Core.lua.L);
		lua_setglobal(Core.lua.L, "__resources");
	}
	map_init(&manager->resources);
	tic_resources_new_type(manager, "image");
	tic_resources_new_type(manager, "tileset");
	tic_resources_new_type(manager, "tilemap");
	tic_resources_new_type(manager, "font");
	tic_resources_new_type(manager, "sound");

	return 1;
}

enum {
	TIC_LOADER_IMAGE = 1,
	TIC_LOADER_TILESET,
	TIC_LOADER_TILEMAP
};

int tic_resources_load(tc_ResourceManager *manager, const char *path) {
	map_init(&manager->resources);
	if (Core.lua.L) {
		lua_newtable(Core.lua.L);
		lua_setglobal(Core.lua.L, "__resources");
	}
	cJSON *json = tic_json_open(path);
	if (!json) return 0;

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


			cJSON_ArrayForEach(resource, el) {
				const char *name = resource->string;
				// const char *path = tic_json_get_string(image, "path");
				int is_lua = tic_json_get_boolean(resource, "lua");
				tc_Resource res;
				res.type = 0;
				res.deps_count = 0;
				res.name = NULL;
				res.path = NULL;
				res.L = NULL;
				if (is_lua) {
					res.type = TIC_RESOURCE_LUA;
					res.L = Core.lua.L;
					lua_settop(res.L, 0);
					lua_pushstring(res.L, el->string);
					lua_pushstring(res.L, name);
				}

				// TRACELOG("%s %s", el->string, name);

				switch(loader) {
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
				}
				// if (loader == TIC_LOADER_IMAGE) tic_resources_image_loader(manager, &res, resource);

				tic_resources_add(manager, el->string, name, res);
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

void tic_resources_new_type(tc_ResourceManager *manager, const char *type) {
	map_resource_t res;
	map_init(&res);
	map_set(&manager->resources, type, res);
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

tc_bool tic_resources_add(tc_ResourceManager *manager, const char *type, const char *name, tc_Resource resource) {
	// hashmap_item *item = hashmap_get(manager->resources, type);
	// if (item && item->data) {
	// 	hashmap *map = (hashmap*)item->data;
	// 	hashmap_item *resource = hashmap_get(*map, name);
	// 	if (resource) return;
	// 	// else hashmap_set()
	// }
	// tc_Resource *resource = map_get()
	map_resource_t *res = map_get(&manager->resources, type);
	if (!res) return 0;

	if (!resource.name) {
		resource.name = calloc(strlen(name), 1);
		strcpy(resource.name, name);
	}
	map_set(res, name, resource);
	if (resource.type & TIC_RESOURCE_LUA) {
		int top = lua_gettop(resource.L);
		// tc_Image *image = lua_touserdata(resource.L, 3);
		// if (image) TRACELOG("userdata %d", top);
		// TRACELOG("%s %s %d", type, name, top);
		lua_getglobal(resource.L, "__resources");
		lua_getfield(resource.L, -1, type);
		// top = lua_gettop(resource.L);
		// TRACELOG("%d", top);
		if (!lua_isnil(resource.L, -1)) {
			// TRACELOG("%s %s %d", type, name, top);
			lua_pushvalue(resource.L, top);
			lua_setfield(resource.L, -2, name);
		}
	}
	
	return 1;
}

tc_Resource* tic_resources_get(tc_ResourceManager *manager, const char *type, const char *name) {
	tc_Resource *res = NULL;
	map_resource_t *res_type = map_get(&manager->resources, type);
	if (res_type) {
		res = map_get(res_type, name);
		if (res->type & TIC_RESOURCE_LUA) {
			lua_getglobal(res->L, "__resources");
			lua_getfield(res->L, -1, type);
			if (!lua_isnil(res->L, -1)) {
				lua_getfield(res->L, -1, name);
				lua_pushvalue(res->L, -1);
			} else lua_pushnil(res->L);
		}
		// TRACELOG("%s %s", type, res->name);
	}

	return res;
}

static tc_Image* tic_lua_image_loader(lua_State *L, const char *path) {
	
}

tc_Image *tic_resources_image_loader(tc_ResourceManager *manager, tc_Resource *res, cJSON *json) {
	tc_Image *image = NULL;
	const char *path = tic_json_get_string(json, "path");
	if (res->type & TIC_RESOURCE_LUA)	{
		image = lua_newuserdata(res->L, sizeof(*image));
		luaL_setmetatable(res->L, LUA_CLASS(tc_Image));
	}
	else image = malloc(sizeof(*image));

	if (image) {
		res->path = malloc(strlen(path));
		strcpy(res->path, path);
		*image = tic_image_load(path);
		res->image = image;
		res->type |= TIC_RESOURCE_IMAGE;
	}

	return image;
}

tc_Tileset *tic_resources_tileset_loader(tc_ResourceManager *manager, tc_Resource *res, cJSON *json) {
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
		return NULL;
	}

	if (res->type & TIC_RESOURCE_LUA) {
		tileset = lua_newuserdata(res->L, sizeof(*tileset));
		luaL_setmetatable(res->L, LUA_CLASS(tc_Tileset));
	} else tileset = malloc(sizeof(*tileset));

	if (tileset) {
		// TRACELOG("teste");
		res->path = malloc(strlen(path));
		strcpy(res->path, path);
		res->deps[res->deps_count++] = resource;
		*tileset = tic_tileset_create(*resource->image, tilew, tileh);
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
		res->tileset = tileset;
		res->type |= TIC_RESOURCE_TILESET;
	}

	return tileset;
}

tc_Tilemap *tic_resources_tilemap_loader(tc_ResourceManager *manager, tc_Resource *res, cJSON *json) {
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
		return NULL;
	}

	if (res->type & TIC_RESOURCE_LUA) {
		tilemap = lua_newuserdata(res->L, sizeof(*tilemap));
		luaL_setmetatable(res->L, LUA_CLASS(tc_Tilemap));
	} else tilemap = malloc(sizeof(*tilemap));

	if (tilemap) {
		// TRACELOG("teste");
		res->path = malloc(strlen(path));
		strcpy(res->path, path);
		res->deps[res->deps_count++] = resource;
		*tilemap = tic_tilemap_create(resource->tileset, width, height);
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
		res->tilemap = tilemap;
		res->type |= TIC_RESOURCE_TILEMAP;
	}

	return tilemap;
}

void tic_resources_tileset_store(tc_ResourceManager *manager, const char *name) {
	// TRACELOG("%s", name);
	map_resource_t *res = map_get(&manager->resources, "tileset");
	if (!res) return;

	tc_Resource *resource = map_get(res, name);
	if (!resource) return;

	cJSON *root = NULL;
	if (resource->path) root = tic_json_open(resource->path);
	else root = tic_json_create();

	tc_Tileset *tileset = resource->tileset;
	int deps_count = resource->deps_count;
	tic_json_set_number(root, "tilewidth", tileset->tilesize.x);
	tic_json_set_number(root, "tileheight", tileset->tilesize.y);
	tic_json_set_number(root, "imagewidth", tileset->image.width);
	tic_json_set_number(root, "imageheight", tileset->image.height);
	// if (editor->image_name) tic_json_set_string(root, "image", editor->image_name);
	tic_json_set_number(root, "columns", tileset->columns);
	tic_json_set_number(root, "tilecount", tileset->tilecount);

	if (deps_count > 0) {
		tic_json_set_string(root, "image", resource->deps[0]->name);
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
	if (!resource->path) {
		resource->path = malloc(strlen("tileset.json"));
		strcpy(resource->path, "tileset.json");
	}

	tic_json_save(resource->path, root);
	tic_json_delete(root);
}

void tic_resources_tilemap_store(tc_ResourceManager *manager, const char *name) {
	TRACELOG("%s", name);
	map_resource_t *res = map_get(&manager->resources, "tilemap");
	if (!res) return;

	tc_Resource *resource = map_get(res, name);
	if (!resource) return;


	cJSON *root = NULL;
	if (resource->path) root = tic_json_open(resource->path);
	else root = tic_json_create();

	tc_Tilemap *tilemap = resource->tilemap;
	int deps_count = resource->deps_count;
	tic_json_set_number(root, "width", tilemap->width);
	tic_json_set_number(root, "height", tilemap->height);
	// if (editor->image_name) tic_json_set_string(root, "image", editor->image_name);
	tic_json_set_number(root, "count", tilemap->count);
	tic_json_set_number(root, "tilewidth", tilemap->grid.x);
	tic_json_set_number(root, "tileheight", tilemap->grid.y);
	// tic_json_set_number(root, "tilecount", tileset->tilecount);

	if (deps_count > 0) {
		tic_json_set_string(root, "tileset", resource->deps[0]->name);
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
	if (!resource->path) {
		resource->path = malloc(strlen("tilemap.json"));
		strcpy(resource->path, "tilemap.json");
	}

	tic_json_save(resource->path, root);
	tic_json_delete(root);
}

tc_bool tic_resources_add_image(tc_ResourceManager *manager, const char *name, tc_Image *image) {
	tc_Resource res = {0};
	res.image = image;
	res.type |= TIC_RESOURCE_IMAGE;

	return tic_resources_add(manager, "image", name, res);
}

tc_Image* tic_resources_get_image(tc_ResourceManager *manager, const char *name) {
	tc_Resource *res = tic_resources_get(manager, "image", name);
	if (res) return res->image;

	return NULL;
}

void tic_resource_remove_image(tc_ResourceManager *manager, const char * name) {
	tc_Resource *res = tic_resources_get(manager, "image", name);
	map_resource_t *type = map_get(&manager->resources, "image");
	if (!res) return;
	map_remove(type, name);
	tc_Image *image = res->image;
	tic_image_destroy(image);
}