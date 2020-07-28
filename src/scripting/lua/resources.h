#include "../../tico.h"

static int tic_lua_resources_load(lua_State *L) {
	int top = lua_gettop(L);
	// TRACELOG("%d", top);
	if (top <= 0) tic_resources_init(&Core.resources);	
	else {
		const char *path = luaL_checkstring(L, 1);
		tic_resources_load(&Core.resources, path);
	}
	

	return 0;
}

static int tic_lua_resources_add(lua_State *L) {
	const char *type = luaL_checkstring(L, 1);
	const char *name = luaL_checkstring(L, 2);

	tc_Image *image = luaL_testudata(L, 3, IMAGE_CLASS);
	tc_Tileset *tileset = luaL_testudata(L, 3, TILESET_CLASS);
	tc_Tilemap *tilemap = luaL_testudata(L, 3, TILEMAP_CLASS);
	tc_Resource res = {0};
	res.type = TIC_RESOURCE_LUA;
	res.L = L;
	int resp = 0;
	if (image) {
		res.image = image;
		res.type |= TIC_RESOURCE_IMAGE;
	} else if (tileset) {
		res.tileset = tileset;
		res.type |= TIC_RESOURCE_TILESET;
	} else if (tilemap) {
		res.tilemap = tilemap;
		res.type |= TIC_RESOURCE_TILEMAP;
	}

	
	resp = tic_resources_add(&Core.resources, type, name, res);
	// if () {
	// 	resp = 1;
		
	// }

	lua_pushboolean(L, resp);

	return 1;
}

static int tic_lua_resources_get(lua_State *L) {

	const char *type = luaL_checkstring(L, 1);
	const char *name = luaL_checkstring(L, 2);

	tic_resources_get(&Core.resources, type, name);
	

	return 1;
}

int luaopen_resources(lua_State *L) {
	luaL_Reg reg[] = {
		{"load", tic_lua_resources_load},
		{"add", tic_lua_resources_add},
		{"get", tic_lua_resources_get},
		{NULL, NULL}
	};

	luaL_newlib(L, reg);

	return 1;
}