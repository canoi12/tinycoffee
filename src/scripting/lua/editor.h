#include "../../tico.h"

#if defined(TICO_LUA_IMPLEMENTATION)

static int tic_lua_editor_load(lua_State *L) {
	tic_editor_load(&Core.editor);

	return 0;
}

static int tic_lua_editor_default(lua_State *L) {
	tic_editor_default(&Core.editor);

	return 0;
}

static int tic_lua_editor_open(lua_State *L) {
	const char *type = luaL_checkstring(L, 1);
	const char *name = luaL_checkstring(L, 2);

	tic_editor_open(&Core.editor, type, name);

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

static int tic_lua_editor_is_game_open(lua_State *L) {
	int res = tic_editor_is_game_open(&Core.editor);
	lua_pushboolean(L, res);
	return 1;
}

static int tic_lua_editor_game_canvas(lua_State *L) {
	tc_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_CLASS);
	tic_editor_set_game_canvas(&Core.editor, canvas);

	return 1;
}

int luaopen_editor(lua_State *L) {
	luaL_Reg reg[] = {
		{"load", tic_lua_editor_load},
		{"default", tic_lua_editor_default},
		// {"update", NULL},
		{"draw", tic_lua_editor_draw},
		{"begin_draw", tic_lua_editor_begin_draw},
		{"end_draw", tic_lua_editor_end_draw},
		{"set_tilemap", tic_lua_editor_set_tilemap},
		{"set_tileset", tic_lua_editor_set_tileset},
		{"open", tic_lua_editor_open},
		{"game_open", tic_lua_editor_is_game_open},
		{"game_canvas", tic_lua_editor_game_canvas},
		{NULL, NULL}
	};

	luaL_newlib(L, reg);
	return 1;
}

#endif