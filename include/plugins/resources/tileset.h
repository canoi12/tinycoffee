#ifndef TICO_PLUGIN_RESOURCE_TILESET_H
#define TICO_PLUGIN_RESOURCE_TILESET_H

TIC_API tc_ResourcePlugin tico_plugin_resource_tileset();
TIC_API tc_Resource tico_plugin_resource_tileset_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json);
TIC_API cJSON* tico_plugin_resource_tileset_store(tc_ResourceManager *manager, tc_Resource *res);

#endif

#if defined(TICO_PLUGIN_RESOURCES_IMPLEMENTATION)

tc_ResourcePlugin tico_plugin_resource_tileset() {
	// return tico_plugin_resource_json();
	return tico_plugin_resource_init(tico_plugin_resource_tileset_loader, tico_plugin_resource_tileset_store);
}

tc_Resource tico_plugin_resource_tileset_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json) {
	tc_Tileset *tileset = NULL;

	tc_Lua *lua = manager->lua;
	if (lua)	{
		tileset = lua_newuserdata(lua->L, sizeof(*tileset));
		luaL_setmetatable(lua->L, LUA_CLASS(tc_Tileset));
	}
	else tileset = malloc(sizeof(*tileset));

	if (tileset) {
		tico_tileset_from_json(tileset, json);
		res->data = tileset;
	}
	if (!res->status) res->status |= TIC_RESOURCE_LOADED;

	return *res;

	// tc_Tileset *tileset = NULL;
	// const char *path = res->path;
	// cJSON *root = json;
	// int tilew = tico_json_get_number(root, "tilewidth", 0);
	// int tileh = tico_json_get_number(root, "tileheight",0);

	// tc_Plugin *plugin = tico_plugin_get("lua");
	// if (plugin) {
	// 	tc_Lua *lua = plugin->data;
	// 	tileset = lua_newuserdata(lua->L, sizeof(*tileset));
	// 	luaL_setmetatable(lua->L, LUA_CLASS(tc_Tileset));
	// } else tileset = malloc(sizeof(*tileset));

	// if (tileset) {
	// 	tc_ResourceDep *dep = map_get(&res->dep, "image");
	// 	tc_Image *image = dep->res->data;
	// 	*tileset = tico_tileset_create(image, tilew, tileh);
	// 	cJSON *bitmask = tico_json_get_array(root, "bitmasks",0);
	// 	cJSON *el;
	// 	int i = 0;
	// 	if (bitmask) {
	// 		cJSON_ArrayForEach(el, bitmask) {
	// 			if (i >= tileset->tilecount) break;
	// 			tileset->bitmasks[i] = el->valueint;
	// 			i++;
	// 		}
	// 	}
	// 	res->data = tileset;
	// 	res->status = TIC_RESOURCE_LOADED;
	// }

	// return *res;
}

cJSON* tico_plugin_resource_tileset_store(tc_ResourceManager *manager, tc_Resource *res) {
	// int size = strlen("tileset##") + strlen(res->name);
 //  char nm[size];
 //  sprintf(nm, "tileset##%s", res->name);

	// if (!res) return NULL;

	// cJSON *root = tico_json_create();

	// tc_Tileset *tileset = res->data;
	// // int deps_count = res->deps_count;
	// tico_json_add_number(root, "tilewidth", tileset->tilesize.x);
	// tico_json_add_number(root, "tileheight", tileset->tilesize.y);
	// tico_json_add_number(root, "imagewidth", tileset->image->width);
	// tico_json_add_number(root, "imageheight", tileset->image->height);
	// tico_json_add_number(root, "columns", tileset->columns);
	// tico_json_add_number(root, "tilecount", tileset->tilecount);

	// cJSON *bitmask = tico_json_get_array(root, "bitmasks", 0);
	// if (bitmask) {
	// 	for (int i = 0; i < tileset->tilecount; i++) {
	// 		cJSON_DeleteItemFromArray(bitmask, 0);
	// 	}
	// } else {
	// 	bitmask = tico_json_create_array(NULL);
	// 	tico_json_add_array(root, "bitmasks", bitmask);
	// }

	// for (int i = 0; i < tileset->tilecount; i++) {
	// 	cJSON *n = cJSON_CreateNumber(tileset->bitmasks[i]);
	// 	cJSON_InsertItemInArray(bitmask, i, n);
	// }

	// if (!res->path[0]) {
	// 	strcpy(res->path, "tileset_");
	// 	strcat(res->path, res->name);
	// 	strcat(res->path, ".json");
	// }

	// // tico_json_save(res->path, root);
	// return root;
	return NULL;
}

#endif