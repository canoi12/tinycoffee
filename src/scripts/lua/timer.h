#ifndef TICO_LUA_TIME_H
#define TICO_LUA_TIME_H

TCDEF int luaopen_timer(lua_State *L);

#endif

#if defined(TICO_LUA_IMPLEMENTATION)

static int tc_lua_get_fps(lua_State *L) {
  lua_pushnumber(L, tc_get_fps());
  return 1;
}

static int tc_lua_get_delta(lua_State *L) {
  lua_pushnumber(L, tc_get_delta());
  return 1;
}

TCDEF int luaopen_timer(lua_State *L) {
  luaL_Reg reg[] = {
    {"fps", tc_lua_get_fps},
    {"delta", tc_lua_get_delta},
    {NULL, NULL}
  };
  luaL_newlib(L, reg);
//   luaL_register(L, NULL, reg);
//   lua_pushvalue(L, -1);

  return 1;
}

#endif
