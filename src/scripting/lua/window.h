#ifndef TICO_WINDOW_H
#define TICO_WINDOW_H

#include "../../tico.h"

TIC_API int luaopen_window(lua_State *L);

#endif

#if defined(TICO_LUA_IMPLEMENTATION)

static int tic_lua_window_pos(lua_State *L) {
	int x, y;
	tic_window_get_pos(&x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);

	return 2;
}

static int tic_lua_window_title(lua_State *L) {
	const char *title = luaL_optstring(L, 1, tic_window_get_title());
	tic_window_set_title(title);
	lua_pushstring(L, title);

	return 1;
}

static int tic_lua_window_size(lua_State *L) {
	int w = luaL_optinteger(L, 1, tic_window_get_width());
	int h = luaL_optinteger(L, 2, tic_window_get_height());
	tic_window_set_size(w, h);
	lua_pushinteger(L, w);
	lua_pushinteger(L, h);

	return 2;
}

int luaopen_window(lua_State *L) {
	luaL_Reg reg[] = {
		{"getPos", tic_lua_window_pos},
		{"getTitle", tic_lua_window_title},
		{"setTitle", tic_lua_window_title},
		{"getSize", tic_lua_window_size},
		{"setSize", tic_lua_window_size},
		{NULL, NULL}
	};
	luaL_newlib(L, reg);


	return 1;
}

#endif