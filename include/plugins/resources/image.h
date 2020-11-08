#ifndef TICO_PLUGIN_RESOURCE_IMAGE_H
#define TICO_PLUGIN_RESOURCE_IMAGE_H

TIC_API tc_ResourcePlugin tico_plugin_resource_image();
TIC_API tc_Resource tico_plugin_resource_image_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json);
TIC_API cJSON* tico_plugin_resource_image_store(tc_ResourceManager *manager, tc_Resource *res);

#endif

#if defined(TICO_PLUGIN_RESOURCES_IMPLEMENTATION)

tc_ResourcePlugin tico_plugin_resource_image() {
	return tico_plugin_resource_init(tico_plugin_resource_image_loader, tico_plugin_resource_image_store);
}

tc_Resource tico_plugin_resource_image_loader(tc_ResourceManager *manager, tc_Resource* res, cJSON* json) {
	tc_Image *image = NULL;
	// const char *path = tico_json_get_string(json, "path");
	// TRACELOG("%s", path);
	// const char *path = res->path;

	const char *path = tico_json_get_string(json, "path", 0);


	// int lua = tico_json_get_boolean(json, "lua");
	tc_Lua *lua = manager->lua;
	if (lua)	{
		image = lua_newuserdata(lua->L, sizeof(*image));
		luaL_setmetatable(lua->L, LUA_CLASS(tc_Image));
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

#endif