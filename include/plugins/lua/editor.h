#include "../../tico.h"

static int tico_lua_editor_test(lua_State *L) {

	TRACELOG("testandow");

	return 0;
}

int luaopen_editor(lua_State *L) {
	luaL_Reg reg[] = {
		{"test", tico_lua_editor_test},
		{NULL, NULL}
	};

	luaL_newlib(L, reg);

	return 1;
}