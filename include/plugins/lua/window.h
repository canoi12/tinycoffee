#ifndef TICO_WINDOW_H
#define TICO_WINDOW_H

#include "../../tico.h"

TIC_API int luaopen_window(lua_State *L);

#endif

#if defined(TICO_LUA_IMPLEMENTATION)

static int tico_lua_window_pos(lua_State *L) {
	int x, y;
	tico_window_get_pos(&x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);

	return 2;
}

static int tico_lua_window_title(lua_State *L) {
	const char *title = luaL_optstring(L, 1, tico_window_get_title());
	tico_window_set_title(title);
	lua_pushstring(L, title);

	return 1;
}

static int tico_lua_window_size(lua_State *L) {
	int w = luaL_optinteger(L, 1, tico_window_get_width());
	int h = luaL_optinteger(L, 2, tico_window_get_height());
	tico_window_set_size(w, h);
	lua_pushinteger(L, w);
	lua_pushinteger(L, h);

	return 2;
}

static int tico_lua_window_width(lua_State *L) {
	int w = luaL_optinteger(L, 1, tico_window_get_width());
	tico_window_set_width(w);

	lua_pushinteger(L, w);
	return 1;
}

static int tico_lua_window_height(lua_State *L) {
	int h = luaL_optinteger(L, 1, tico_window_get_height());
	tico_window_set_height(h);

	lua_pushinteger(L, h);
	return 1;
}

int luaopen_window(lua_State *L) {
	luaL_Reg reg[] = {
		{"getPos", tico_lua_window_pos},
		{"getTitle", tico_lua_window_title},
		{"setTitle", tico_lua_window_title},
		{"getSize", tico_lua_window_size},
		{"setSize", tico_lua_window_size},
		{"getWidth", tico_lua_window_width},
		{"setWidth", tico_lua_window_width},
		{"getHeight", tico_lua_window_height},
		{"setHeight", tico_lua_window_height},
		{NULL, NULL}
	};
	luaL_newlib(L, reg);


	return 1;
}

#endif