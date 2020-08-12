#include "../../tico.h"


#if defined(TICO_LUA_IMPLEMENTATION)

/******************
 * Tileset
 ******************/

static int tic_lua_tileset_new(lua_State *L) {
	tc_Tileset *tileset = lua_newuserdata(L, sizeof(*tileset));
	luaL_setmetatable(L, TILESET_CLASS);

	tc_Image *image = luaL_checkudata(L, 1, IMAGE_CLASS);
	int width = luaL_optinteger(L, 2, 16);
	int height = luaL_optinteger(L, 3, 16);

	*tileset = tic_tileset_create(image, width, height);

	return 1;
}

static int tic_lua_tileset_load(lua_State *L) {
	tc_Tileset *tileset = lua_newuserdata(L, sizeof(*tileset));
	luaL_setmetatable(L, TILESET_CLASS);

	const char *path = luaL_checkstring(L, 1);
	tc_Image *image = luaL_checkudata(L, 2, IMAGE_CLASS);
	*tileset = tic_tileset_load(path, image);

	return 1;
}

static int tic_lua_tileset__gc(lua_State *L) {
	tc_Tileset *tileset = luaL_checkudata(L, 1, TILESET_CLASS);

	tic_tileset_destroy(tileset);
	return 0;
}

static int tic_lua_tileset_draw(lua_State *L) {
	tc_Tileset *tileset = luaL_checkudata(L, 1, TILESET_CLASS);
	int index = luaL_optinteger(L, 2, 0);
	int x = luaL_optinteger(L, 3, 0);
	int y = luaL_optinteger(L, 4, 0);
	tc_Color color = lua_optcolor(L, 5, WHITE);

	tic_tileset_draw(*tileset, index, x, y, color);

	return 0;
}

static int tic_lua_tileset_get_rect(lua_State *L) {
	tc_Tileset *tileset = luaL_checkudata(L, 1, TILESET_CLASS);
	int index = luaL_optinteger(L, 2, 0);

	// int index = tic_tileset_get_from_bitmask(*tileset, bitmask);
	tc_Rectf rect = tic_tileset_get_rect(*tileset, index);
	lua_newtable(L);
	for (int i = 0; i < 4; i++) {
		lua_pushinteger(L, rect.data[i]);
		lua_rawseti(L, -2, i);
	}

	return 1;
}

static int tic_lua_tileset_from_bitmask(lua_State *L) {
	tc_Tileset *tileset = luaL_checkudata(L, 1, TILESET_CLASS);
	int bitmask = luaL_optinteger(L, 2, -1);

	int index = tic_tileset_get_from_bitmask(*tileset, bitmask);

	lua_pushinteger(L, index);
	return 1;
}

static int tic_lua_tileset_get_rect_from_bitmask(lua_State *L) {
	tc_Tileset *tileset = luaL_checkudata(L, 1, TILESET_CLASS);
	return 0;
}

int luaopen_tileset(lua_State *L) {
	luaL_Reg reg[] = {
		{"__call", tic_lua_tileset_new},
		{"draw", tic_lua_tileset_draw},
		{"get_rect", tic_lua_tileset_get_rect},
		{"from_bitmask", tic_lua_tileset_from_bitmask},
		// {"get_rect_from_bitmask", },
		{"__gc", tic_lua_tileset__gc},
		{NULL, NULL}
	};

	// luaL_newlib(L, reg);

	luaL_newmetatable(L, TILESET_CLASS);
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

	return 1;
}

/**************************
 * Tilemap
 **************************/

static int tic_lua_tilemap_new(lua_State *L) {
	tc_Tilemap *map = lua_newuserdata(L, sizeof(*map));
	luaL_setmetatable(L, TILEMAP_CLASS);

	tc_Tileset *tileset = luaL_checkudata(L, 1, TILESET_CLASS);
	int w = luaL_optinteger(L, 2, 16);
	int h = luaL_optinteger(L, 3, 16);

	*map = tic_tilemap_create(tileset, w, h);

	return 1;
}

static int tic_lua_tilemap_load(lua_State *L) {
	tc_Tilemap *map = lua_newuserdata(L, sizeof(*map));
	luaL_setmetatable(L, TILEMAP_CLASS);

	const char *path = luaL_checkstring(L, 1);
	tc_Tileset *tileset = luaL_checkudata(L, 2, TILESET_CLASS);

	*map = tic_tilemap_load(path, tileset);

	return 1;
}

static int tic_lua_tilemap_draw(lua_State *L) {
	tc_Tilemap *map = luaL_checkudata(L, 1, TILEMAP_CLASS);

	tic_tilemap_draw(*map);

	return 0;
}

static int tic_lua_tilemap_update(lua_State *L) {
	tc_Tilemap *map = luaL_checkudata(L, 1, TILEMAP_CLASS);

	for (int i = 0; i < map->count; i++) {
		if (map->data[i] >= 0) map->data[i] = tic_tilemap_autotile(*map, i);
	}

	return 0;
}

static int tic_lua_tilemap_insert(lua_State *L) {
	tc_Tilemap *map = luaL_checkudata(L, 1, TILEMAP_CLASS);

	int index = luaL_optinteger(L, 2, -1);
	tic_tilemap_insert(map, index);
	// TRACELOG("%d", map->data[index]);

	return 0;
}

static int tic_lua_tilemap_has_tile(lua_State *L) {
	tc_Tilemap *map = luaL_checkudata(L, 1, TILEMAP_CLASS);
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	int has = tic_tilemap_has_tile(*map, x, y);

	lua_pushboolean(L, has);

	return 1;
}

int luaopen_tilemap(lua_State *L) {
	luaL_Reg reg[] = {
		{"__call", tic_lua_tilemap_new},
		{"update", tic_lua_tilemap_update},
		{"draw", tic_lua_tilemap_draw},
		{"insert", tic_lua_tilemap_insert},
		{"has_tile", tic_lua_tilemap_has_tile},
		{NULL, NULL}
	};

	luaL_newmetatable(L, TILEMAP_CLASS);
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  return 1;
}

/*********************
 * Camera
 *********************/

static int tic_lua_camera_new(lua_State *L) {

	float x = luaL_optnumber(L, 1, 0);
	float y = luaL_optnumber(L, 2, 0);
	float w = luaL_optnumber(L, 3, Core.window.width);
	float h = luaL_optnumber(L, 4, Core.window.height);

	tc_Camera *camera = lua_newuserdata(L, sizeof(*camera));
	luaL_setmetatable(L, CAMERA_CLASS);

	*camera = tic_camera_create(x, y, w, h);

	return 1;
}

static int tic_lua_camera_attach(lua_State *L) {
	tc_Camera *camera = luaL_checkudata(L, 1, CAMERA_CLASS);

	tic_camera_attach(*camera);

	return 0;
}

static int tic_lua_camera_detach(lua_State *L) {
	tc_Camera *camera = luaL_checkudata(L, 1, CAMERA_CLASS);
	tic_camera_detach();

	return 0;
}

static int tic_lua_camera_move(lua_State *L) {
	tc_Camera *camera = luaL_checkudata(L, 1, CAMERA_CLASS);

	float x = luaL_optnumber(L, 2, camera->area.x);
	float y = luaL_optnumber(L, 3, camera->area.y);

	camera->area.x = x;
	camera->area.y = y;

	return 0;
}

int luaopen_camera(lua_State *L) {
	luaL_Reg reg[] = {
		{"__call", tic_lua_camera_new},
		{"attach", tic_lua_camera_attach},
		{"detach", tic_lua_camera_detach},
		{"move", tic_lua_camera_move},
		{NULL, NULL}
	};

	luaL_newmetatable(L, CAMERA_CLASS);
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  return 1;
}

int luaopen_engine(lua_State *L) {
	luaL_Reg reg[] = {
		{"newCamera", tic_lua_camera_new},
		{"new_tileset", tic_lua_tileset_new},
		{"load_tileset", tic_lua_tileset_load},
		{"new_tilemap", tic_lua_tilemap_new},
		{"load_tilemap", tic_lua_tilemap_load},
		{NULL, NULL}
	};

	luaL_newlib(L, reg);

	return 1;
}

#endif
