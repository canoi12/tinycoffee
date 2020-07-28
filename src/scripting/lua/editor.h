#include "../../tico.h"

#if defined(TICO_LUA_IMPLEMENTATION)

static int tic_lua_editor_load(lua_State *L) {
	tic_editor_load(&Core.editor);

	return 0;
}

static int tic_lua_editor_draw(lua_State *L) {
	tic_editor_draw(&Core.editor);

	return 0;
}

static int tic_lua_editor_begin_draw(lua_State *L) {
	int res = tic_editor_begin_draw(&Core.editor);

	lua_pushboolean(L, res);
	return 1;
}

static int tic_lua_editor_end_draw(lua_State *L) {
	tic_editor_end_draw(&Core.editor);

	return 0;
}

static int tic_lua_editor_set_tilemap(lua_State *L) {
	int type = lua_type(L, 1);
	tc_Tilemap *map = NULL;
	char *name = NULL;

	if (type == LUA_TUSERDATA) map = luaL_checkudata(L, 1, TILEMAP_CLASS);
	else if (type == LUA_TSTRING) name = (char*)luaL_checkstring(L, 1);
	else return 0;

	tic_editor_set_tilemap(&Core.editor, name, map);

	return 0;
}

static int tic_lua_editor_set_tileset(lua_State *L) {
	int type = lua_type(L, 1);
	tc_Tileset *tileset = NULL;
	char *name = NULL;

	if (type == LUA_TUSERDATA) tileset = luaL_checkudata(L, 1, TILESET_CLASS);
	else if (type == LUA_TSTRING) name = (char*)luaL_checkstring(L, 1);
	else return 0;

	tic_editor_set_tileset(&Core.editor, name, tileset);

	return 0;
}

int luaopen_editor(lua_State *L) {
	luaL_Reg reg[] = {
		{"load", tic_lua_editor_load},
		// {"update", NULL},
		{"draw", tic_lua_editor_draw},
		{"begin_draw", tic_lua_editor_begin_draw},
		{"end_draw", tic_lua_editor_end_draw},
		{"set_tilemap", tic_lua_editor_set_tilemap},
		{"set_tileset", tic_lua_editor_set_tileset},
		{NULL, NULL}
	};

	luaL_newlib(L, reg);
	return 1;
}

#endif