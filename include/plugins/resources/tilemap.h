#ifndef TICO_PLUGIN_RESOURCE_TILEMAP_H
#define TICO_PLUGIN_RESOURCE_TILEMAP_H

TIC_API tc_ResourcePlugin tico_plugin_resource_tilemap();
TIC_API tc_Resource tico_plugin_resource_tilemap_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json);
TIC_API cJSON* tico_plugin_resource_tilemap_store(tc_ResourceManager *manager, tc_Resource *res);

#endif

#if defined(TICO_PLUGIN_RESOURCES_IMPLEMENTATION)

#include "../lua/json.h"

tc_ResourcePlugin tico_plugin_resource_tilemap() {
	return tico_plugin_resource_json();
	// return tico_plugin_resource_init(tico_plugin_resource_tilemap_loader, tico_plugin_resource_tilemap_store);
}

tc_Resource tico_plugin_resource_tilemap_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json) {
	// tc_Tilemap *tilemap = NULL;
	// const char *path = tico_json_get_string(json, "path");
	const char *path = res->path;

	// TRACELOG("%s", path);

	// cJSON *root = tico_json_open(path);
	cJSON *root = json;
	if (manager->lua) {
		tc_Lua *lua = manager->lua;
		tico_lua_json_new_object(lua->L, json);
	}
	res->status |= TIC_RESOURCE_LOADED;
	res->data = json;
	// int width = tico_json_get_number(root, "width");
	// int height = tico_json_get_number(root, "height");
	// const char *tileset_name = tico_json_get_string(root, "tileset");
	// if (res->lua) {
	// 	tilemap = lua_newuserdata(res->L, sizeof(*tilemap));
	// 	luaL_setmetatable(res->L, LUA_CLASS(tc_Tilemap));
	// } else tilemap = malloc(sizeof(*tilemap));

	// if (tilemap) {
	// 	tc_ResourceDep *dep = map_get(&res->dep, "tileset");
	// 	tc_Tileset *tileset = dep->res->data;
	// 	*tilemap = tico_tilemap_create(tileset, width, height);
	// 	cJSON *data = tico_json_get_array(root, "data");
	// 	cJSON *el;
	// 	int i = 0;
	// 	cJSON_ArrayForEach(el, data) {
	// 		if (i > tilemap->count) break;
	// 		tilemap->data[i] = el->valueint;
	// 		i++;
	// 	}

	// 	// tico_json_delete(root);
	// 	res->data = tilemap;
	// 	res->status = TIC_RESOURCE_LOADED;
	// 	// strcpy(res->type, "tilemap");
	// }

	return *res;
}

cJSON* tico_plugin_resource_tilemap_store(tc_ResourceManager *manager, tc_Resource *resource) {
	// int size = strlen("tilemap//") + strlen(resource->name);
 //  char nm[size];
 //  sprintf(nm, "tilemap//%s", resource->name);
	// // TRACELOG("%s", nm);

	if (!resource) return NULL;


	cJSON *root = resource->data;
	tico_json_save("eeeeita.json", root);
	// cJSON *root = tico_json_create();

	// tc_Tilemap *tilemap = resource->data;
	// int deps_count = resource->deps_count;
	// tico_json_set_number(root, "width", tilemap->width);
	// tico_json_set_number(root, "height", tilemap->height);
	// tico_json_set_number(root, "count", tilemap->count);
	// tico_json_set_number(root, "tilewidth", tilemap->grid.x);
	// tico_json_set_number(root, "tileheight", tilemap->grid.y);


	// // list_iter_t iter = list_iter(&resource->deps);
	// // tc_ResourceDep *dep;
	// // while((dep = list_next(&iter))) {
	// // 	tico_json_set_string(root, dep->name, dep->res->name);
	// // }

	// cJSON *data = tico_json_get_array(root, "data");
	// if (data) {
	// 	for (int i = 0; i < tilemap->count; i++) {
	// 		cJSON_DeleteItemFromArray(data, 0);
	// 	}
	// } else {
	// 	data = tico_json_create_array();
	// 	tico_json_set_array(root, "data", data);
	// }

	// for (int i = 0; i < tilemap->count; i++) {
	// 	cJSON *n = cJSON_CreateNumber(tilemap->data[i]);
	// 	cJSON_InsertItemInArray(data, i, n);
	// }

	// if (!resource->path[0]) {
	// 	strcpy(resource->path, "tilemap.json");
	// }

	// tico_json_save(resource->path, root);
	
	return root;
}

#endif