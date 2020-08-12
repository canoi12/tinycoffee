#ifndef TICO_PLUGIN_RESOURCE_H
#define TICO_PLUGIN_RESOURCE_H

#include "../tico.h"

typedef tc_Resource(*PluginResourceLoader)(tc_ResourceManager*, tc_Resource*, cJSON*);
typedef cJSON*(*PluginResourceStore)(tc_ResourceManager*, tc_Resource*);

struct tc_ResourcePlugin {
	PluginResourceLoader loader;
	PluginResourceStore store;
};

TIC_API tc_ResourcePlugin tic_plugin_resource_create(PluginResourceLoader loader, PluginResourceStore store);

/*****************
 * Image
 *****************/

TIC_API tc_ResourcePlugin tic_plugin_resource_image();
TIC_API tc_Resource tic_plugin_resource_image_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json);
TIC_API cJSON* tic_plugin_resource_image_store(tc_ResourceManager *manager, tc_Resource *res);

/****************
 * Tileset
 ****************/

TIC_API tc_ResourcePlugin tic_plugin_resource_tileset();
TIC_API tc_Resource tic_plugin_resource_tileset_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json);
TIC_API cJSON* tic_plugin_resource_tileset_store(tc_ResourceManager *manager, tc_Resource *res);

/****************
 * Tilemap
 ****************/

TIC_API tc_ResourcePlugin tic_plugin_resource_tilemap();
TIC_API tc_Resource tic_plugin_resource_tilemap_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json);
TIC_API cJSON* tic_plugin_resource_tilemap_store(tc_ResourceManager *manager, tc_Resource *res);

#endif

#if defined(TICO_PLUGIN_IMPLEMENTATION)

tc_ResourcePlugin tic_plugin_resource_init(PluginResourceLoader loader, PluginResourceStore store) {
	tc_ResourcePlugin plugin = {0};
	plugin.loader = loader;
	plugin.store = store;

	return plugin;
}

/*****************
 * Image
 *****************/

tc_ResourcePlugin tic_plugin_resource_image() {
	return tic_plugin_resource_init(tic_plugin_resource_image_loader, tic_plugin_resource_image_store);
}

tc_Resource tic_plugin_resource_image_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json) {
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

cJSON* tic_plugin_resource_image_store(tc_ResourceManager *manager, tc_Resource *res) {}

/****************
 * Tileset
 ****************/

tc_ResourcePlugin tic_plugin_resource_tileset() {
	return tic_plugin_resource_init(tic_plugin_resource_tileset_loader, tic_plugin_resource_tileset_store);
}

tc_Resource tic_plugin_resource_tileset_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json) {
	tc_Tileset *tileset = NULL;
	const char *path = tic_json_get_string(json, "path");

	cJSON *root = tic_json_open(path);
	int tilew = tic_json_get_number(root, "tilewidth");
	int tileh = tic_json_get_number(root, "tileheight");
	const char *image_name = tic_json_get_string(root, "image");
	tc_Resource *resource = NULL;
	if (image_name) resource = tic_resources_get(manager, "image", image_name);
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
		strcpy(res->path, path);

		tc_ResourceDep dep = tic_resources_new_dep("image", resource);
		list_push(&res->deps, dep);
		tc_Image *image = resource->data;
		*tileset = tic_tileset_create(image, tilew, tileh);
		cJSON *bitmask = tic_json_get_array(root, "bitmasks");
		cJSON *el;
		int i = 0;
		if (bitmask) {
			cJSON_ArrayForEach(el, bitmask) {
				if (i >= tileset->tilecount) break;
				tileset->bitmasks[i] = el->valueint;
				i++;
			}
		}

		tic_json_delete(root);
		res->data = tileset;
		strcpy(res->type, "tileset");
	}

	return *res;
}

cJSON* tic_plugin_resource_tileset_store(tc_ResourceManager *manager, tc_Resource *res) {
	int size = strlen("tileset##") + strlen(res->name);
  char nm[size];
  sprintf(nm, "tileset##%s", res->name);

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
	tic_json_set_number(root, "columns", tileset->columns);
	tic_json_set_number(root, "tilecount", tileset->tilecount);


	list_iter_t iter = list_iter(&res->deps);
	tc_ResourceDep *dep;
	while((dep = list_next(&iter))) {
		tic_json_set_string(root, dep->name, dep->res->name);
	}


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

	if (!res->path[0]) {
		strcpy(res->path, "tileset_");
		strcat(res->path, res->name);
		strcat(res->path, ".json");
	}

	tic_json_save(res->path, root);
	return root;
}

/****************
 * Tilemap
 ****************/

tc_ResourcePlugin tic_plugin_resource_tilemap() {
	return tic_plugin_resource_init(tic_plugin_resource_tilemap_loader, tic_plugin_resource_tilemap_store);
}

tc_Resource tic_plugin_resource_tilemap_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json) {
	tc_Tilemap *tilemap = NULL;
	const char *path = tic_json_get_string(json, "path");

	cJSON *root = tic_json_open(path);
	int width = tic_json_get_number(root, "width");
	int height = tic_json_get_number(root, "height");
	const char *tileset_name = tic_json_get_string(root, "tileset");
	tc_Resource *resource = NULL;
	if (tileset_name) resource = tic_resources_get(manager, "tileset", tileset_name);
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
			i++;
		}

		tic_json_delete(root);
		res->data = tilemap;
		strcpy(res->type, "tilemap");
	}

	return *res;
}

cJSON* tic_plugin_resource_tilemap_store(tc_ResourceManager *manager, tc_Resource *resource) {
	int size = strlen("tilemap##") + strlen(resource->name);
  char nm[size];
  sprintf(nm, "tilemap##%s", resource->name);
	// TRACELOG("%s", nm);

	if (!resource) return NULL;


	cJSON *root = tic_json_create();

	tc_Tilemap *tilemap = resource->data;
	int deps_count = resource->deps_count;
	tic_json_set_number(root, "width", tilemap->width);
	tic_json_set_number(root, "height", tilemap->height);
	tic_json_set_number(root, "count", tilemap->count);
	tic_json_set_number(root, "tilewidth", tilemap->grid.x);
	tic_json_set_number(root, "tileheight", tilemap->grid.y);


	list_iter_t iter = list_iter(&resource->deps);
	tc_ResourceDep *dep;
	while((dep = list_next(&iter))) {
		tic_json_set_string(root, dep->name, dep->res->name);
	}

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

	if (!resource->path[0]) {
		strcpy(resource->path, "tilemap.json");
	}

	tic_json_save(resource->path, root);
	
	return root;
}

#endif